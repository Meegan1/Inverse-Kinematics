//
// Created by Jake Meegan on 28/04/2020.
//

#ifndef ASSIGNMENT_1_MEDIAWIDGET_H
#define ASSIGNMENT_1_MEDIAWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QFileDialog>
#include "Rendering/Engine.h"

class MediaWidget : public QWidget {
Q_OBJECT
    Engine *engine;
    QPushButton *play;
    QPushButton *edit;
    QPushButton *save;
public:
    MediaWidget(QWidget *parent, Engine *engine) : QWidget(parent), engine(engine) {
        play = new QPushButton("Pause", this);
        edit = new QPushButton("Edit Joint", this);
        save = new QPushButton("Save", this);

        connect(engine, SIGNAL(playChanged(bool)), this, SLOT(updatePlayButton(bool)));
        connect(engine, SIGNAL(editChanged(bool)), this, SLOT(updateEditButton(bool)));

        connect(play, SIGNAL(released()), this, SLOT(playPressed()));
        connect(edit, SIGNAL(released()), this, SLOT(editPressed()));
        connect(save, SIGNAL(released()), this, SLOT(savePressed()));

        // create layout
        QBoxLayout *layout = new QVBoxLayout();
        this->setLayout(layout);
        layout->setMargin(0);

        layout->addWidget(edit);
        layout->addWidget(play);
        layout->addWidget(save);
    }

private slots:
    void playPressed() {
        engine->togglePlay();
    }

    void editPressed() {
        engine->toggleEdit();
    }

    void savePressed() {
        QFileDialog dialog(this);
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setNameFilter(tr("*.bvh"));
        dialog.setFileMode(QFileDialog::AnyFile);
        if(dialog.exec()) {
            engine->bvh.Save(dialog.selectedFiles().first().toStdString().c_str());
        }
    }

    void updatePlayButton(bool isPlaying) {
        if(!isPlaying)
            play->setText("Play");
        else
            play->setText("Pause");
    }

    void updateEditButton(bool isEditing) {
        if(!isEditing)
            edit->setText("Edit Joint");
        else
            edit->setText("Stop Editing Joint");
    }
};
#endif //ASSIGNMENT_1_MEDIAWIDGET_H
