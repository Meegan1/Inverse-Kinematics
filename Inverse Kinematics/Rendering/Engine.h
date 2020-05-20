//
// Created by Jake Meegan on 03/03/2020.
//

#ifndef NATURAL_PHENOMENA_ENGINE_H
#define NATURAL_PHENOMENA_ENGINE_H
#define GL_SILENCE_DEPRECATION

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QElapsedTimer>
#include <QTimer>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "QKeyEvent"
#include "qdebug.h"
#include "Input.h"
#include "../Animation/BVH.h"
#include "GL/freeglut.h"
#include <glm/mat4x2.hpp>



// physics fixed timestep
#define DELTA_TIME 0.01f

class Engine : public QOpenGLWidget, protected QOpenGLFunctions {
Q_OBJECT
public:
	BVH bvh;

    explicit Engine(QWidget *parent);

	void selectJoint(BVH::Joint* joint);

	Eigen::Vector3f getTargetPosition();

	void togglePlay();
	bool isPlaying();
	void toggleEdit();
	bool isEditing();
    void loadBVH(const char* file);
    void setFrame(int frame);

signals:
    void playChanged(bool value);
    void editChanged(bool value);
    void frameChanged(int value);

protected:
    /*
     * OpenGL Methods
     */
    void initializeGL() override;

    void startLoop();

    void loop();

    void resizeGL(int w, int h) override;

    void paintGL() override;

    /*
     * Input Events
     */
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    /*
     * Mouse Events
     */
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    /*
     * Class members
     */
    QTimer timer;
    QElapsedTimer elapsed_timer;
    GLfloat delta_accumulator;
    Camera camera;
    Input input;
    QPoint last_m_pos;
    bool is_editing = false;
    bool is_playing = true;
    float frame = 0;
	int prev_frame = -1;
	glm::vec2 window_size;
	GLubyte* pixels;
	glm::mat4 target_position;
	BVH::Joint* selected_joint;
	
private:
};


#endif //NATURAL_PHENOMENA_ENGINE_H
