#pragma warning(disable: 4018)
#include <glm/vec3.hpp>
#include <glm/mat4x2.hpp>
#ifndef  _BVH_H_
#define  _BVH_H_


#include <vector>
#include <map>
#include <string>
#include <Eigen/Eigen>
#include <iostream>


using namespace std;


class BVH {
public:


    enum ChannelEnum {
        X_ROTATION, Y_ROTATION, Z_ROTATION,
        X_POSITION, Y_POSITION, Z_POSITION
    };
    struct Joint;


    struct Channel {

        Joint *joint;


        ChannelEnum type;


        int index;
    };


    struct Joint {

        string name;

        int index;


        Joint *parent;

        vector<Joint *> children;


        double offset[3];


        bool has_site;

        double site[3];


        vector<Channel *> channels;
    };


public:

    bool is_load_success;


    string file_name;
    string motion_name;


    int num_channel;
    vector<Channel *> channels;
    vector<Joint *> joints;
    map<string, Joint *> joint_index;


    int num_frame;
    double interval;
    double *motion;


public:

    BVH();

    BVH(const char *bvh_file_name);

    ~BVH();


    void Clear();


    void Load(const char *bvh_file_name);

    void Save(const char *bvh_file_name);

    const string PrintJoints(std::vector<Joint *> joints, int depth = 0);
    const string PrintJoint(Joint *joint, int depth = 0);
    const string PrintDepth(int depth);

    const string PrintChannelName(ChannelEnum &type);

public:


    bool IsLoadSuccess() const { return is_load_success; }


    const string &GetFileName() const { return file_name; }

    const string &GetMotionName() const { return motion_name; }


    const int GetNumJoint() const { return joints.size(); }

    const Joint *GetJoint(int no) const { return joints[no]; }

    const int GetNumChannel() const { return channels.size(); }

    const Channel *GetChannel(int no) const { return channels[no]; }

    const Joint *GetJoint(const string &j) const {
        map<string, Joint *>::const_iterator i = joint_index.find(j);
        return (i != joint_index.end()) ? (*i).second : NULL;
    }

    const Joint *GetJoint(const char *j) const {
        map<string, Joint *>::const_iterator i = joint_index.find(j);
        return (i != joint_index.end()) ? (*i).second : NULL;
    }


    int GetNumFrame() const { return num_frame; }

    double GetInterval() const { return interval; }

    double GetMotion(int f, int c) const { return motion[f * num_channel + c]; }


    void SetMotion(int f, int c, double v) { motion[f * num_channel + c] = v; }

    int AddFrame(const double *new_frame);

    bool isClose(BVH &other, int frame_no) {
        for(int i = 0; i < joints.size(); i++) {
            if(abs((getVectorPosition(joints[i], frame_no, 1.0f) - other.getVectorPosition(other.joints[i], 0)).norm()) > 0.7f)
                return false;
        }

        return true;
    }

    void interpolateOther(BVH &other, int frame_no) {
        auto *old_frame = new double[num_channel];
        std::memcpy(old_frame, &motion[(frame_no) * num_channel], sizeof(motion[0]) * num_channel);
        AddFrame(old_frame);

        for(int i = 0; i < joints.size(); i++) {
            executeIK(other.getVectorPosition(other.joints[i], 0), joints[i], frame_no);
        }
        std::memcpy(&motion[(frame_no + 1) * num_channel], &motion[(frame_no) * num_channel], sizeof(motion[0]) * num_channel);

        std::memcpy(&motion[(frame_no) * num_channel], old_frame, sizeof(motion[0]) * num_channel);
    }

public:


    void RenderFigure(int frame_no, float scale = 1.0f);


    static void RenderFigure(const Joint *root, const double *data, float scale = 1.0f);


    static void RenderBone(float x0, float y0, float z0, float x1, float y1, float z1, float bRadius = 0.1);
	
	glm::mat4 getPosition(const Joint* root, int frame_no, float scale = 1.0f);

	Eigen::Vector3f getVectorPosition(const Joint* root, int frame_no, float scale = 1.0f) {
        glm::vec3 position = getPosition(root, frame_no, 1.0f) * glm::vec4(0, 0, 0, 1.0f);
        return {position.x, position.y, position.z};
	}

    void addRotation(Joint *joint, int frame_no, const Eigen::Vector3f &value) {
        for (auto channel : joint->channels) {
            if (channel->type == X_ROTATION)
                SetMotion(frame_no, channel->index, GetMotion(frame_no, channel->index) + value.x());
            else if (channel->type == Y_ROTATION)
                SetMotion(frame_no, channel->index, GetMotion(frame_no, channel->index) + value.y());
            else if (channel->type == Z_ROTATION)
                SetMotion(frame_no, channel->index, GetMotion(frame_no, channel->index) + value.z());
        }
    }

    void executeIK(const Eigen::Vector3f &targetPosition, Joint* end_effector, int frame_no) {
        calculateJacobianIK(targetPosition, end_effector, frame_no, Eigen::Vector3f(0.0f, 0.0f, 1.0f));
        calculateJacobianIK(targetPosition, end_effector, frame_no, Eigen::Vector3f(0.0f, 1.0f, 0.0f));
        calculateJacobianIK(targetPosition, end_effector, frame_no, Eigen::Vector3f(1.0f, 0.0f, 0.0f));
	}

	void calculateJacobianIK(const Eigen::Vector3f &targetPosition, Joint* end_effector, int frame_no, const Eigen::Vector3f &axis) {
	    if(end_effector->parent == nullptr)
	        return;

        Eigen::Vector3f distanceVector = (getVectorPosition(end_effector, frame_no, 1.0f) - targetPosition);
	    if(abs((distanceVector.norm())) > 0.1f) {
            Eigen::VectorXf d0 = getDeltaOrientation(targetPosition, end_effector, frame_no, axis) * 0.1f;

            std::vector<Joint*> joints;
            Joint* current_joint = end_effector->parent;
            while(current_joint->parent != nullptr) {
                joints.emplace(joints.begin(), current_joint);
                // climb to parent
                current_joint = current_joint->parent;
            }

            for(int i = 0; i < joints.size(); i++) {
                addRotation(joints[i], frame_no, axis * (d0[i] * (pow(i, 3) / (float) pow(joints.size(), 3))));
            }
	    }
	}

	Eigen::VectorXf getDeltaOrientation(const Eigen::Vector3f &targetPosition, Joint* end_effector, int frame_no, const Eigen::Vector3f &axis) {
	    Eigen::MatrixX3f Jt = calculateJacobian(end_effector, frame_no, axis);
	    Eigen::Vector3f V = targetPosition - getVectorPosition(end_effector, frame_no, 1.0f);
	    Eigen::VectorXf d0;
	    d0.resize(Jt.cols());
	    return d0 = Jt * V;
	}

    Eigen::MatrixX3f calculateJacobian(Joint* end_effector, int frame_no, const Eigen::Vector3f &axis) {
        Eigen::Matrix3Xf matrix;

        Eigen::Vector3f E = getVectorPosition(end_effector, frame_no, 1.0f);

        std::vector<Joint*> joints;
        Joint* current_joint = end_effector->parent;
        while(current_joint->parent != nullptr) {
            joints.emplace(joints.begin(), current_joint);
            // climb to parent
            current_joint = current_joint->parent;
        }

        for(auto &joint : joints) {
            Eigen::Vector3f A = getVectorPosition(joint, frame_no, 1.0f);
            Eigen::Vector3f J_A = axis.cross(E - A);

            matrix.conservativeResize(matrix.rows(), matrix.cols() + 1);
            matrix.col(matrix.cols() - 1) = J_A;
        }

        return matrix.transpose();
    }
};


#endif
