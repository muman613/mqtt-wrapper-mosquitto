#include <QDebug>
#include <QApplication>
#ifdef MQTTWRAPPER_DEBUG_LOGGING
#include <QFile>
#include <QMutex>
#include <QTextStream>
#endif

#include "mqttWrapper.h"
#include "mainwindow.h"

#ifdef MQTTWRAPPER_DEBUG_LOGGING
namespace {
QMutex logMutex;

void fileMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message) {
    QMutexLocker locker(&logMutex);
    QFile logFile("mqttwrapper.log");

    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&logFile);
    stream << qFormatLogMessage(type, context, message) << Qt::endl;
}
}
#endif

int main(int argc, char *argv[]) {
#ifdef MQTTWRAPPER_DEBUG_LOGGING
    qSetMessagePattern("[%{time yyyy-MM-dd hh:mm:ss.zzz}] [%{type}] %{message}");
    qInstallMessageHandler(fileMessageHandler);
#endif

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
