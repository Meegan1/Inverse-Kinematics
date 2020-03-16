#include <QApplication>
#include "Rendering/Engine.h"
#include "AnimationTree.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    Engine engine(NULL);
    engine.resize(600, 600);
    engine.show();

	AnimationTree animationTree(NULL, &engine);
	animationTree.resize(300, 300);
	animationTree.show();
    return app.exec();
}
