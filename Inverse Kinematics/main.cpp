#include <QApplication>
#include "Rendering/Engine.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    Engine engine(NULL);
    engine.resize(600, 600);
    engine.show();
    return app.exec();
}
