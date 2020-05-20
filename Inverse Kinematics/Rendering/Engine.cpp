//
// Created by Jake Meegan on 03/03/2020.
//

#include "Engine.h"
#include "../Animation/BVH.h"
#include "../Video.h"
#include <QThread>

Engine::Engine(QWidget *parent) : QOpenGLWidget(parent), camera({0, 4, 10}), bvh("../arms_up_test.bvh") {
    setWindowTitle("Animation Viewer");
}

void Engine::selectJoint(BVH::Joint* joint)
{
	selected_joint = joint;

	target_position = bvh.getPosition(joint, frame, 1.0f);
}

void Engine::initializeGL() {
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST); // enable z-buffering
    // set lighting parameters
//    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL); // enable colour

    // set background colour to purple
    glClearColor(0.5f, 0.5f, 1.0f, 1.0f);

    startLoop(); // start game loop
}

void Engine::startLoop() {
    // start elapsed timer
    elapsed_timer.start();

    // start game loop
    connect(&timer, &QTimer::timeout, this, QOverload<>::of(&Engine::loop));
    timer.start();
}

void Engine::loop() {
    // get time from previous frame
    GLfloat frame_time = elapsed_timer.nsecsElapsed() * 0.000000001;
    elapsed_timer.restart();
    delta_accumulator += frame_time;

    camera.update();

    if(is_playing) {
        frame += frame_time / bvh.interval;
        if(frame >= bvh.num_frame)
            frame = 0;

        emit frameChanged((100 * frame)/bvh.num_frame);
        target_position = bvh.getPosition(selected_joint, frame, 1.0f);
    }

	// if ((int) frame != prev_frame) {
	// 	prev_frame = frame;
	// 	Video::create_ppm("tmp", frame, window_size.x, window_size.y, 255, 4, pixels);
	// }

	if(is_editing) {
	    bvh.executeIK(getTargetPosition(), selected_joint, frame);
    }

    if(input.keyboard.KEY_W)
        camera.zoom(frame_time * 5);
    if(input.keyboard.KEY_S)
        camera.zoom(-frame_time * 5);
    if(input.keyboard.KEY_A)
        camera.pan(frame_time * 5, 0);
    if(input.keyboard.KEY_D)
        camera.pan(-frame_time * 5, 0);

    // call window/opengl to update
    update();
}

void Engine::resizeGL(int w, int h) {
    const float aspectRatio = (float) w / (float) h;

    // resize viewport to window size
    glViewport(0, 0, w, h);

    // set projection matrix to glFrustum based on window size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-aspectRatio, aspectRatio, -1.0, 1.0, 1, 100.0);

	window_size = { w, h };

	pixels = static_cast<GLubyte*>(malloc(4 * window_size.x * window_size.y));
}

void Engine::paintGL() {
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // enable lighting
    glEnable(GL_LIGHTING);

    // set model view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMultMatrixf(&camera.getView()[0][0]);

    /*
     * Draw objects
     */

    bvh.RenderFigure(frame, 1.0f);

	glPushMatrix();
		glPushAttrib(GL_CURRENT_BIT);
		    if(is_editing)
			    glColor3f(1, 0, 0);
            else
                glColor3f(0, 0, 1);
			glMultMatrixf(&target_position[0][0]);
//			glTranslatef(getTargetPosition().x(), getTargetPosition().y(), getTargetPosition().z());
			gluSphere(gluNewQuadric(), 0.3, 10, 10);
		glPopAttrib();
	glPopMatrix();

    GLfloat lightPos[] = {0, 30.0f, -10.0f, 0};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);


	glReadPixels(0, 0, window_size.x, window_size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void Engine::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_W)
        input.keyboard.KEY_W = true;
    else if(event->key() == Qt::Key_A)
        input.keyboard.KEY_A = true;
    else if(event->key() == Qt::Key_S)
        input.keyboard.KEY_S = true;
    else if(event->key() == Qt::Key_D)
        input.keyboard.KEY_D = true;
    else if(event->key() == Qt::Key_Space)
        input.keyboard.KEY_SPACE = true;
}

void Engine::keyReleaseEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_W)
        input.keyboard.KEY_W = false;
    else if(event->key() == Qt::Key_A)
        input.keyboard.KEY_A = false;
    else if(event->key() == Qt::Key_S)
        input.keyboard.KEY_S = false;
    else if(event->key() == Qt::Key_D)
        input.keyboard.KEY_D = false;
    else if(event->key() == Qt::Key_Space)
        input.keyboard.KEY_SPACE = false;
}

void Engine::mouseMoveEvent(QMouseEvent *event) {
    float sensitivity = 0.01;
    if (event->buttons() & Qt::LeftButton) { // left click to control rotation
        float xoffset = event->x() - last_m_pos.x();
        float yoffset = event->y() - last_m_pos.y();

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        camera.rotate(xoffset, yoffset);
    }
	
	if (event->buttons() & Qt::RightButton)
	{
		float xoffset = event->x() - last_m_pos.x();
		float yoffset = event->y() - last_m_pos.y();

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		glm::mat4 inverse = glm::inverse(target_position);
		target_position = glm::translate(target_position, glm::vec3(inverse * glm::vec4(camera.right(), 0.0f)) * xoffset);
		target_position = glm::translate(target_position, glm::vec3(inverse * glm::vec4(-camera.up(), 0.0f)) * yoffset);
	}

    last_m_pos = event->pos(); // store last mouse position
}

void Engine::mousePressEvent(QMouseEvent *event) {
    last_m_pos = event->pos(); // clear mouse position upon click
}

Eigen::Vector3f Engine::getTargetPosition() {
    glm::vec3 position = target_position * glm::vec4(0, 0, 0, 1.0f);
    return {position.x, position.y, position.z};
}

void Engine::togglePlay() {
    this->is_playing = !this->is_playing;

    if(isEditing() && isPlaying())
        toggleEdit();

    emit playChanged(isPlaying());
}

bool Engine::isPlaying() {
    return this->is_playing;
}

void Engine::toggleEdit() {
    this->is_editing = !this->is_editing;

    if(isEditing() && isPlaying())
        togglePlay();

    emit editChanged(isEditing());
}

bool Engine::isEditing() {
    return this->is_editing;
}

void Engine::loadBVH(const char* file) {
    // pause animation
    this->is_playing = false;
    this->is_playing = false;
    emit playChanged(isPlaying());
    emit editChanged(isEditing());

    // set frame to 0
    this->frame = 0;
    emit frameChanged(0);

    // set to new animation
    this->bvh.Clear();
    this->bvh.Load(file);
    this->selected_joint = bvh.joints[0];
}

void Engine::setFrame(int frame) {
    this->frame = frame;
}