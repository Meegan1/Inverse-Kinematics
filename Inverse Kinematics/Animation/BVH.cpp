
#include <fstream>
#include <string>

#include "BVH.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <deque>


BVH::BVH() {
    motion = NULL;
    Clear();
}

BVH::BVH(const char *bvh_file_name) {
    motion = NULL;
    Clear();

    Load(bvh_file_name);
}

BVH::~BVH() {
    Clear();
}

void BVH::Clear() {
    unsigned int i;
    for (i = 0; i < channels.size(); i++)
        delete channels[i];
    for (i = 0; i < joints.size(); i++)
        delete joints[i];
    if (motion != NULL)
        delete motion;

    is_load_success = false;

    file_name = "";
    motion_name = "";

    num_channel = 0;
    channels.clear();
    joints.clear();
    joint_index.clear();

    num_frame = 0;
    interval = 0.0;
    motion = NULL;
}


void BVH::Load(const char *bvh_file_name) {
#define  BUFFER_LENGTH  1024*4

    ifstream file;
    char line[BUFFER_LENGTH];
    char *token;
    char separater[] = " :,\t";
    vector<Joint *> joint_stack;
    Joint *joint = NULL;
    Joint *new_joint = NULL;
    bool is_site = false;
    double x, y, z;
    int i, j;

    Clear();

    file_name = bvh_file_name;
    const char *mn_first = bvh_file_name;
    const char *mn_last = bvh_file_name + strlen(bvh_file_name);
    if (strrchr(bvh_file_name, '\\') != NULL)
        mn_first = strrchr(bvh_file_name, '\\') + 1;
    else if (strrchr(bvh_file_name, '/') != NULL)
        mn_first = strrchr(bvh_file_name, '/') + 1;
    if (strrchr(bvh_file_name, '.') != NULL)
        mn_last = strrchr(bvh_file_name, '.');
    if (mn_last < mn_first)
        mn_last = bvh_file_name + strlen(bvh_file_name);
    motion_name.assign(mn_first, mn_last);

    file.open(bvh_file_name, ios::in);
    if (file.is_open() == 0) return;


    while (!file.eof()) {

        if (file.eof()) goto bvh_error;


        file.getline(line, BUFFER_LENGTH);
        token = strtok(line, separater);


        if (token == NULL) continue;


        if (strcmp(token, "{") == 0) {

            joint_stack.push_back(joint);
            joint = new_joint;
            continue;
        }

        if (strcmp(token, "}") == 0) {

            joint = joint_stack.back();
            joint_stack.pop_back();
            is_site = false;
            continue;
        }


        if ((strcmp(token, "ROOT") == 0) ||
            (strcmp(token, "JOINT") == 0)) {

            new_joint = new Joint();
            new_joint->index = joints.size();
            new_joint->parent = joint;
            new_joint->has_site = false;
            new_joint->offset[0] = 0.0;
            new_joint->offset[1] = 0.0;
            new_joint->offset[2] = 0.0;
            new_joint->site[0] = 0.0;
            new_joint->site[1] = 0.0;
            new_joint->site[2] = 0.0;
            joints.push_back(new_joint);
            if (joint)
                joint->children.push_back(new_joint);


            token = strtok(NULL, "");
            while (*token == ' ') token++;
            new_joint->name = token;


            joint_index[new_joint->name] = new_joint;
            continue;
        }


        if ((strcmp(token, "End") == 0)) {
            new_joint = joint;
            is_site = true;
            continue;
        }


        if (strcmp(token, "OFFSET") == 0) {

            token = strtok(NULL, separater);
            x = token ? atof(token) : 0.0;
            token = strtok(NULL, separater);
            y = token ? atof(token) : 0.0;
            token = strtok(NULL, separater);
            z = token ? atof(token) : 0.0;


            if (is_site) {
                joint->has_site = true;
                joint->site[0] = x;
                joint->site[1] = y;
                joint->site[2] = z;
            } else {
                joint->offset[0] = x;
                joint->offset[1] = y;
                joint->offset[2] = z;
            }
            continue;
        }


        if (strcmp(token, "CHANNELS") == 0) {

            token = strtok(NULL, separater);
            joint->channels.resize(token ? atoi(token) : 0);


            for (i = 0; i < joint->channels.size(); i++) {

                Channel *channel = new Channel();
                channel->joint = joint;
                channel->index = channels.size();
                channels.push_back(channel);
                joint->channels[i] = channel;


                token = strtok(NULL, separater);
                if (strcmp(token, "Xrotation") == 0)
                    channel->type = X_ROTATION;
                else if (strcmp(token, "Yrotation") == 0)
                    channel->type = Y_ROTATION;
                else if (strcmp(token, "Zrotation") == 0)
                    channel->type = Z_ROTATION;
                else if (strcmp(token, "Xposition") == 0)
                    channel->type = X_POSITION;
                else if (strcmp(token, "Yposition") == 0)
                    channel->type = Y_POSITION;
                else if (strcmp(token, "Zposition") == 0)
                    channel->type = Z_POSITION;
            }
        }


        if (strcmp(token, "MOTION") == 0)
            break;
    }


    file.getline(line, BUFFER_LENGTH);
    token = strtok(line, separater);
    if (strcmp(token, "Frames") != 0) goto bvh_error;
    token = strtok(NULL, separater);
    if (token == NULL) goto bvh_error;
    num_frame = atoi(token);

    file.getline(line, BUFFER_LENGTH);
    token = strtok(line, ":");
    if (strcmp(token, "Frame Time") != 0) goto bvh_error;
    token = strtok(NULL, separater);
    if (token == NULL) goto bvh_error;
    interval = atof(token);

    num_channel = channels.size();
    motion = new double[num_frame * num_channel];


    for (i = 0; i < num_frame; i++) {
        file.getline(line, BUFFER_LENGTH);
        token = strtok(line, separater);
        for (j = 0; j < num_channel; j++) {
            if (token == NULL)
                goto bvh_error;
            motion[i * num_channel + j] = atof(token);
            token = strtok(NULL, separater);
        }
    }


    file.close();


    is_load_success = true;

    return;

    bvh_error:
    file.close();
}

void BVH::Save(const char *bvh_file_name) {
    ofstream file(bvh_file_name, std::ofstream::out | std::ofstream::trunc);

    file << "HIERARCHY" << std::endl;

    file << "ROOT " << joints[0]->name << std::endl;
    file << "{" << std::endl;
    file << "\tOFFSET " << std::to_string(joints[0]->offset[0]) << " " << std::to_string(joints[0]->offset[1]) << " "
         << std::to_string(joints[0]->offset[2]) << std::endl;
    file << "\tCHANNELS " << joints[0]->channels.size();
    for(auto &channel : joints[0]->channels) {
        file << " " << PrintChannelName(channel->type);
    }
    file << std::endl;
    file << PrintJoints(joints[0]->children, 2);
    file << "}" << std::endl;

    file << "MOTION" << std::endl;
    file << "Frames: " << num_frame << std::endl;
    file << "Frame Time: " << std::to_string(interval) << std::endl;

    for(unsigned long i = 0; i < num_frame; i++) {
        for(unsigned long j = 0; j < num_channel; j++)
            file << std::to_string(motion[i * num_channel + j]) << " ";

        file << std::endl;
    }
    file.close();
}


const std::string BVH::PrintJoint(Joint * joint, int depth) {
    std::string tab = PrintDepth(depth);
    std::string tab_1 = PrintDepth(depth - 1);
    std::stringstream string;
    string << tab_1 <<"JOINT " << joint->name << std::endl;
    string << tab_1 << "{" << std::endl;
    string << tab << "OFFSET " << std::to_string(joint->offset[0]) << " " << std::to_string(joint->offset[1]) << " "
           << std::to_string(joint->offset[2]) << std::endl;
    string << tab << "CHANNELS " << joint->channels.size();
    for(auto &channel : joint->channels) {
        string << " " << PrintChannelName(channel->type);
    }
    string << std::endl;
    if(!joint->has_site)
        string << PrintJoints(joint->children, depth + 1);
    else {
        string << tab << "End Site" << std::endl;
        string << tab << "{" << std::endl;
        string << tab << "\t OFFSET " << std::to_string(joint->site[0]) << " " << std::to_string(joint->site[1]) << " "
               << std::to_string(joint->site[2]) << std::endl;
        string << tab << "}" << std::endl;
    }
    string << tab_1 << "}" << std::endl;

    return string.str();
}

const string BVH::PrintDepth(int depth) {
    std::stringstream string;
    for(int i = 0; i < depth; i++)
        string << "\t";

    return string.str();
}

const std::string BVH::PrintJoints(std::vector<Joint *> joints, int depth) {
    std::stringstream string;
    for(auto &joint : joints) {
        string << PrintJoint(joint, depth);
    }
    return string.str();
}



#include <math.h>
#include "GL/freeglut.h"


void BVH::RenderFigure(int frame_no, float scale) {

    RenderFigure(joints[0], motion + frame_no * num_channel, scale);
}


void BVH::RenderFigure(const Joint *joint, const double *data, float scale) {
    glPushMatrix();


    if (joint->parent == NULL) {
        glTranslatef(data[0] * scale, data[1] * scale, data[2] * scale);
    } else {
        glTranslatef(joint->offset[0] * scale, joint->offset[1] * scale, joint->offset[2] * scale);
    }


    int i;
    for (i = 0; i < joint->channels.size(); i++) {
        Channel *channel = joint->channels[i];
        if (channel->type == X_ROTATION)
            glRotatef(data[channel->index], 1.0f, 0.0f, 0.0f);
        else if (channel->type == Y_ROTATION)
            glRotatef(data[channel->index], 0.0f, 1.0f, 0.0f);
        else if (channel->type == Z_ROTATION)
            glRotatef(data[channel->index], 0.0f, 0.0f, 1.0f);
    }


    if (joint->children.size() == 0) {
        RenderBone(0.0f, 0.0f, 0.0f, joint->site[0] * scale, joint->site[1] * scale, joint->site[2] * scale);
    }

    if (joint->children.size() == 1) {
        Joint *child = joint->children[0];
        RenderBone(0.0f, 0.0f, 0.0f, child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale);
    }

    if (joint->children.size() > 1) {

        float center[3] = {0.0f, 0.0f, 0.0f};
        for (i = 0; i < joint->children.size(); i++) {
            Joint *child = joint->children[i];
            center[0] += child->offset[0];
            center[1] += child->offset[1];
            center[2] += child->offset[2];
        }
        center[0] /= joint->children.size() + 1;
        center[1] /= joint->children.size() + 1;
        center[2] /= joint->children.size() + 1;


        RenderBone(0.0f, 0.0f, 0.0f, center[0] * scale, center[1] * scale, center[2] * scale);


        for (i = 0; i < joint->children.size(); i++) {
            Joint *child = joint->children[i];
            RenderBone(center[0] * scale, center[1] * scale, center[2] * scale,
                       child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale);
        }
    }


    for (i = 0; i < joint->children.size(); i++) {
        RenderFigure(joint->children[i], data, scale);
    }

    glPopMatrix();
}


void BVH::RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, float bRadius) {


    GLdouble dir_x = x1 - x0;
    GLdouble dir_y = y1 - y0;
    GLdouble dir_z = z1 - z0;
    GLdouble bone_length = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);


    static GLUquadricObj *quad_obj = NULL;
    if (quad_obj == NULL)
        quad_obj = gluNewQuadric();
    gluQuadricDrawStyle(quad_obj, GLU_FILL);
    gluQuadricNormals(quad_obj, GLU_SMOOTH);

    glPushMatrix();


    glTranslated(x0, y0, z0);


    double length;
    length = sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
    if (length < 0.0001) {
        dir_x = 0.0;
        dir_y = 0.0;
        dir_z = 1.0;
        length = 1.0;
    }
    dir_x /= length;
    dir_y /= length;
    dir_z /= length;


    GLdouble up_x, up_y, up_z;
    up_x = 0.0;
    up_y = 1.0;
    up_z = 0.0;


    double side_x, side_y, side_z;
    side_x = up_y * dir_z - up_z * dir_y;
    side_y = up_z * dir_x - up_x * dir_z;
    side_z = up_x * dir_y - up_y * dir_x;


    length = sqrt(side_x * side_x + side_y * side_y + side_z * side_z);
    if (length < 0.0001) {
        side_x = 1.0;
        side_y = 0.0;
        side_z = 0.0;
        length = 1.0;
    }
    side_x /= length;
    side_y /= length;
    side_z /= length;


    up_x = dir_y * side_z - dir_z * side_y;
    up_y = dir_z * side_x - dir_x * side_z;
    up_z = dir_x * side_y - dir_y * side_x;


    GLdouble m[16] = {side_x, side_y, side_z, 0.0,
                      up_x, up_y, up_z, 0.0,
                      dir_x, dir_y, dir_z, 0.0,
                      0.0, 0.0, 0.0, 1.0};
    glMultMatrixd(m);


    GLdouble radius = bRadius;
    GLdouble slices = 8.0;
    GLdouble stack = 3.0;


    gluCylinder(quad_obj, radius, radius, bone_length, slices, stack);

    glPopMatrix();
}


glm::mat4 BVH::getPosition(const Joint* end_joint, int frame_no, float scale)
{
	glm::mat4 position(1);
	deque<Joint *> jointList;
	jointList.push_back(const_cast<Joint *>(end_joint));
	
	Joint* iterator = end_joint->parent;
	while(iterator != NULL)
	{
		jointList.push_front(iterator);
		iterator = iterator->parent;
	}

	for (auto &joint : jointList) {
		if (joint->parent == NULL) {
			position = glm::translate(position, glm::vec3(GetMotion(frame_no, 0) * scale, GetMotion(frame_no, 1) * scale, GetMotion(frame_no, 2) * scale));
		}
		else
			position = glm::translate(position, glm::vec3(joint->offset[0] * scale, joint->offset[1] * scale, joint->offset[2] * scale));

		int i;
		for (i = 0; i < joint->channels.size(); i++) {
			Channel* channel = joint->channels[i];
			if (channel->type == X_ROTATION)
				position = glm::rotate(position, glm::radians((float) GetMotion(frame_no, channel->index)), glm::vec3(1.0f, 0.0f, 0.0f));
			else if (channel->type == Y_ROTATION)
				position = glm::rotate(position, glm::radians((float) GetMotion(frame_no, channel->index)), glm::vec3(0.0f, 1.0f, 0.0f));
			else if (channel->type == Z_ROTATION)
				position = glm::rotate(position, glm::radians((float) GetMotion(frame_no, channel->index)), glm::vec3(0.0f, 0.0f, 1.0f));
		}
	}
	
	return position;
}

const string BVH::PrintChannelName(BVH::ChannelEnum &type) {
    if (type == X_ROTATION)
        return "Xrotation";
    else if (type == Y_ROTATION)
        return "Yrotation";
    else if (type == Z_ROTATION)
        return "Zrotation";
    else if (type == X_POSITION)
        return "Xposition";
    else if (type == Y_POSITION)
        return "Yposition";
    else if (type == Z_POSITION)
        return "Zposition";
}

int BVH::AddFrame(const double *new_frame) {
    // create new array with additional frame
    auto* new_motion = new double[(num_frame + 1) * num_channel];

    // copy old array to new array
    std::memcpy(&new_motion[0], &motion[0], sizeof(motion[0]) * num_frame * num_channel);

    // swap pointers
    std::swap(motion, new_motion);

    // allocate new frame
    std::memcpy(&motion[num_frame * num_channel], new_frame, sizeof(motion[0]) * num_channel);

    // update frame counter
    num_frame++;

    return num_frame; // return new frame index
}




