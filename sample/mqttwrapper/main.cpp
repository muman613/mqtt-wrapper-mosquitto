#include <QDebug>
#include <QApplication>

#include "mqttWrapper.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}


//int main(int argc, char * argv[]) {
//    QCoreApplication app(argc, argv);
//    mqttWrapper wrapper;
//
//    if (wrapper.connect("michael-raspi4b", 1883)) {
//        qDebug() << "OK";
//        if (wrapper.subscribe("device/#", 1)) {
//            qDebug() << "OK";
//
//            wrapper.publish("device/update", "File not found", 1, false);
//        }
//    }
//    app.exec();
//
//    return 0;
//}
