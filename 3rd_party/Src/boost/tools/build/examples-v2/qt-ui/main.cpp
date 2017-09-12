#include "hello_world_widget.h"
#include <qapplication.h>

#include <qpushbutton.h>

int main(int argc, char **argv) {
    QApplication a(argc, argv);
    HelloWorldWidget w;
    QObject::connect(static_cast<QObject*>(w.OkButton), SIGNAL(clicked()), &w, SLOT(close()));
    a.setMainWidget(&w);
    w.show();
    return a.exec();
}
