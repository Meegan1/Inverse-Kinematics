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
    QPushButton *generate;
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

        QBoxLayout *buttons = new QVBoxLayout();
        QBoxLayout *add_remove_buttons = new QHBoxLayout();
        buttons->setMargin(0);
        add_remove_buttons->setMargin(0);

        add = new QPushButton("+");
        remove = new QPushButton("-");
        add_remove_buttons->addWidget(remove);
        add_remove_buttons->addWidget(add);

        generate = new QPushButton("Generate Animation");

        layout->addLayout(buttons);
        buttons->addLayout(add_remove_buttons);
        buttons->addWidget(generate);

        connect(add, SIGNAL(released()), this, SLOT(addPose()));
        connect(remove, SIGNAL(released()), this, SLOT(removePose()));
        connect(generate, SIGNAL(released()), this, SLOT(generateAnimation()));
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

    void generateAnimation() {
        if(poses->count() < 2) {

            return;
        }

        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setNameFilter(tr("*.bvh"));
        dialog.setFileMode(QFileDialog::AnyFile);
        if(dialog.exec()) {
            temp_bvh = ((Pose*) poses->item(0))->bvh;

            for(int i = 1; i < poses->count(); i++) {
                BVH other = ((Pose*) poses->item(i))->bvh;
                while(!temp_bvh.isClose(other, temp_bvh.num_frame - 1)) {
                    temp_bvh.interpolateOther(other, temp_bvh.num_frame - 1);
                }
            }
            temp_bvh.Save(dialog.selectedFiles().first().toStdString().c_str());
        }
    }
};

#endif //ASSIGNMENT_1_POSEWIDGET_H
