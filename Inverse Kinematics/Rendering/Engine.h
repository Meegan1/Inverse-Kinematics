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

// physics fixed timestep
#define DELTA_TIME 0.01f

class Engine : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    explicit Engine(QWidget *parent);

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
    float frame = 0;
    BVH bvh;
private:

};


#endif //NATURAL_PHENOMENA_ENGINE_H
