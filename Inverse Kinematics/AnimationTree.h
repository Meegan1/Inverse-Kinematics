#pragma once

#include "QTreeWidget"
#include "Animation/BVH.h"
#include "Rendering/Engine.h"


class AnimationTree : public QTreeWidget
{
	Q_OBJECT
	Engine *engine;
public:
	AnimationTree(QWidget *parent, Engine *engine) : QTreeWidget(parent), engine(engine)
	{
		setColumnCount(1);
		QList<QTreeWidgetItem*> items;
		BVH::Joint *joint = engine->bvh.joints[0];
		QTreeWidgetItem *item = new QTreeWidgetItem(((QTreeWidget*)0, QStringList(QString("%1").arg(joint->name.c_str()))));
		
		for (int i = 0; i < joint->children.size(); i++) {
			addChildren(joint->children[i], item);
		}
		
		insertTopLevelItem(0, item);

		connect(this, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(itemPressed(QTreeWidgetItem*, int)));
	}

	void addChildren(BVH::Joint *joint, QTreeWidgetItem *item)
	{
		QTreeWidgetItem* new_item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("%1").arg(joint->name.c_str())));
		item->addChild(new_item);
		
		for (int i = 0; i < joint->children.size(); i++) {
			addChildren(joint->children[i], new_item);
		}
	}

public slots:
	void itemPressed(QTreeWidgetItem* item, int column)
	{
		engine->selectJoint(const_cast<BVH::Joint *>(engine->bvh.GetJoint(item->text(0).toStdString())));
	}
};