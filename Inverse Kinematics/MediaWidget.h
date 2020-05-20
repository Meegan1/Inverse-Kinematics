//
// Created by Jake Meegan on 28/04/2020.
//

#ifndef ASSIGNMENT_1_MEDIAWIDGET_H
#define ASSIGNMENT_1_MEDIAWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>
#include <QFileDialog>
#include <QSlider>
#include "Rendering/Engine.h"

class MediaWidget : public QWidget {
Q_OBJECT
    Engine *engine;
    QPushButton *play;
	QPushButton *record;
    QPushButton *edit;
    QPushButton *save;
    QPushButton *load;
    QSlider *slider;
public:
    MediaWidget(QWidget *parent, Engine *engine) : QWidget(parent), engine(engine) {
        play = new QPushButton("Pause", this);
		record = new QPushButton("Record", this);
        edit = new QPushButton("Edit Joint", this);
        save = new QPushButton("Save", this);
        load = new QPushButton("Load", this);
        slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(0, 99);


        connect(engine, SIGNAL(playChanged(bool)), this, SLOT(updatePlayButton(bool)));
		connect(engine, SIGNAL(recordChanged(bool)), this, SLOT(updateRecordButton(bool)));
        connect(engine, SIGNAL(editChanged(bool)), this, SLOT(updateEditButton(bool)));
        connect(engine, SIGNAL(frameChanged(int)), this, SLOT(timeChanged(int)));

        connect(play, SIGNAL(released()), this, SLOT(playPressed()));
		connect(record, SIGNAL(released()), this, SLOT(recordPressed()));
        connect(edit, SIGNAL(released()), this, SLOT(editPressed()));
        connect(save, SIGNAL(released()), this, SLOT(savePressed()));
        connect(load, SIGNAL(released()), this, SLOT(loadPressed()));
        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

        // create layout
        QBoxLayout *layout = new QVBoxLayout();
        this->setLayout(layout);
        layout->setMargin(0);

        layout->addWidget(edit);
        layout->addWidget(play);
		layout->addWidget(record);
        layout->addWidget(save);
        layout->addWidget(load);
        layout->addWidget(slider);
    }

private slots:
    void playPressed() {
        engine->togglePlay();
    }

	void recordPressed() {
		engine->toggleRecord();
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

	void updateRecordButton(bool isRecording) {
		if (!isRecording)
			record->setText("Record");
		else
			record->setText("Stop Recording");
	}

    void updateEditButton(bool isEditing) {
        if(!isEditing)
            edit->setText("Edit Joint");
        else
            edit->setText("Stop Editing Joint");
    }

    void loadPressed() {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::ExistingFile);
        if(dialog.exec()) {
            engine->loadBVH(dialog.selectedFiles().first().toStdString().c_str());
        }
    }

    void timeChanged(int value) {
        const QSignalBlocker blocker(slider); // block valueChanged from being triggered
        slider->setValue(value);
    }

    void valueChanged(int value) {
        this->engine->setFrame((value * this->engine->bvh.num_frame) / 100);
    }
};
#endif //ASSIGNMENT_1_MEDIAWIDGET_H
