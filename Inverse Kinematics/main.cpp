#include <QApplication>
#include <QWindow>
#include "Rendering/Engine.h"
#include "AnimationTree.h"
#include "UIWindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    Engine engine(NULL);
    engine.resize(600, 600);
    engine.show();
	UIWindow uiWindow(&engine);

    return app.exec();
}
