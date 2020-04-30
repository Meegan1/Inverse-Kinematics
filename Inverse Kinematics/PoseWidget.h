//
// Created by Jake Meegan on 29/04/2020.
//

#ifndef ASSIGNMENT_1_POSEWIDGET_H
#define ASSIGNMENT_1_POSEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include "Rendering/Engine.h"

struct Pose : public QListWidgetItem {
    BVH bvh;

    explicit Pose(const char* file) : bvh(file), QListWidgetItem() {
        setText(bvh.GetMotionName().c_str());
    }
};

class PoseWidget : public QWidget {
Q_OBJECT
    Engine *engine;
    QListWidget *poses;
    QPushButton *add;
    QPushButton *remove;
    BVH temp_bvh;

public:
    PoseWidget(QWidget *parent, Engine *engine) : QWidget(parent), engine(engine) {
        QBoxLayout *layout = new QVBoxLayout();
        this->setLayout(layout);
        layout->setMargin(0);

        poses = new QListWidget(this);
        poses->setDragDropMode(QAbstractItemView::InternalMove);

        poses->addItem(new Pose("../rest.bvh"));
        poses->addItem(new Pose("../dab.bvh"));

        layout->addWidget(poses);

        QBoxLayout *buttons = new QHBoxLayout();

        add = new QPushButton("+");
        remove = new QPushButton("-");
        buttons->addWidget(remove);
        buttons->addWidget(add);

        layout->addLayout(buttons);

        connect(add, SIGNAL(released()), this, SLOT(addPose()));
        connect(remove, SIGNAL(released()), this, SLOT(removePose()));
    }

private slots:
    void addPose() {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::ExistingFile);
        if(dialog.exec()) {
            Pose *pose = new Pose(dialog.selectedFiles().first().toStdString().c_str());
            poses->addItem(pose);
        }
    };

    void removePose() {
        qDeleteAll(poses->selectedItems());
    };

    void generateInterpolation() {
        temp_bvh = ((Pose*) poses->item(0))->bvh;

        BVH other = ((Pose*) poses->item(1))->bvh;

        while(!temp_bvh.isClose(other, temp_bvh.num_frame - 1)) {
            temp_bvh.interpolateOther(other, temp_bvh.num_frame - 1);
        }
        temp_bvh.Save("../arms_up_test.bvh");
    }
};

#endif //ASSIGNMENT_1_POSEWIDGET_H
