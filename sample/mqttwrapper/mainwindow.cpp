#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "mqttWrapper.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    qDebug() << Q_FUNC_INFO;
    // qInfo() << "endpoint = " << endpoint;

    ui->setupUi(this);

    connect(&mqtt, &mqttWrapper::onMessage, this, &MainWindow::onMessage);
    connect(&mqtt, &mqttWrapper::onConnectionChanged, this, &MainWindow::onConnectionChanged);

    restoreOptions();
}

MainWindow::~MainWindow() {
    qDebug() << Q_FUNC_INFO;
    saveOptions();
    mqtt.disconnect();
    delete ui;
}

void MainWindow::on_pubButton_clicked() {
    qDebug() << Q_FUNC_INFO;
    QString pubTopic = ui->pubTopic->text();
    if (!pubTopic.isEmpty()) {
        auto pubMessage = ui->sendMessageText->toPlainText().toUtf8();
        if (!pubMessage.isEmpty()) {
            mqtt.publish(pubTopic, pubMessage, 1, false);
        } else {
            QMessageBox::critical(this, "mqttgui", "No message specified");
        }
    } else {
        QMessageBox::critical(this, "mqttgui", "No topic specified");
    }
}

void MainWindow::on_subButton_clicked() {
    qDebug() << Q_FUNC_INFO;
    QString subTopic = ui->subTopic->text();

    if (mqtt.subscribe(subTopic, 1)) {
        displayMessage(QString("Subscribed to topic %1").arg(subTopic));
    }
}

void MainWindow::on_unSubButton_clicked() {
    qDebug() << Q_FUNC_INFO;
    QString subTopic = ui->subTopic->text();

    if (mqtt.unsubscribe(subTopic)) {
        displayMessage(QString("Unsubscribed from topic %1").arg(subTopic));
    } else {
        displayMessage(QString("Unable to unsubscribe from topic %1").arg(subTopic));
    }
}

void MainWindow::enableInterface(bool enabled) {
    ui->pubTopic->setEnabled(enabled);
    ui->subTopic->setEnabled(enabled);
    ui->pubButton->setEnabled(enabled);
    ui->unSubButton->setEnabled(enabled);
    ui->subButton->setEnabled(enabled);
    ui->sendMessageText->setEnabled(enabled);
    ui->messageText->setEnabled(enabled);
    ui->loadButton->setEnabled(enabled);
    if (enabled) {
        ui->connectButton->setText("Disconnect");
    } else {
        ui->connectButton->setText("Connect");
    }
}

void MainWindow::on_actionClear_message_text_triggered() { ui->messageText->clear(); }

void MainWindow::on_loadButton_clicked() {
    qDebug() << Q_FUNC_INFO;
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open JSON"), "", tr("JSON Files (*.json)"));

    QFile jsonFile(fileName);
    if (jsonFile.open(QIODevice::ReadOnly)) {
        QByteArray data = jsonFile.readAll();
        ui->sendMessageText->clear();
        ui->sendMessageText->setText(data.data());
    } else {
        qWarning() << "Unable to open file" << fileName;
    }
}

void MainWindow::on_actionAbout_mqttgui_triggered() {
    QMessageBox::about(this, "mqttgui", "Qt Based MQTT using Mosquitto Client");
}

void MainWindow::on_connectButton_clicked() {
    if (mqtt.property("connected").toBool() == false) {
        QString host = ui->brokerCombo->currentText();
        int port = ui->port->text().toInt();

        if (mqtt.connect(host, port)) {
            enableInterface(true);
        }
    } else {
        if (mqtt.disconnect()) {
            enableInterface(false);
        }
    }
}

void MainWindow::onMessage(QString topic, QByteArray payload) {
    displayMessage(QString("%1: %2").arg(topic, QString::fromLocal8Bit(payload)));
}

void MainWindow::restoreOptions() {
    QSettings settings("softwaremagic", "mqttwrapper", this);

    settings.beginGroup("options");
    auto brokers = settings.value("brokers").toString();
    auto broker_list = brokers.split(':', QString::SkipEmptyParts);

    for (const auto broker: broker_list) {
        ui->brokerCombo->addItem(broker);
    }

    auto port = settings.value("broker_port_").toString();
    ui->port->setText(port);

    settings.endGroup();
}

void MainWindow::saveOptions() {
    QSettings settings("softwaremagic", "mqttwrapper", this);
    QStringList broker_list;
    for (int index = 0; index < ui->brokerCombo->count(); index++) {
        auto broker = ui->brokerCombo->itemText(index);
        broker_list.append(broker);
    }
    settings.beginGroup("options");
    settings.setValue("brokers", broker_list.join(':'));
    auto port = ui->port->text().toInt();
    settings.setValue("broker_port_", port);
    settings.endGroup();
}

void MainWindow::displayMessage(const QString &msg) {
    QDateTime ts = QDateTime::currentDateTime();
    auto time_stamp = ts.toString();
    ui->messageText->append(QString("%1 : %2").arg(time_stamp).arg(msg));
    ui->messageText->ensureCursorVisible();
}

void MainWindow::onConnectionChanged(bool connected) {
    if (connected) {
        auto port = mqtt.property("port").toInt();
        auto host = mqtt.property("host").toString();

        displayMessage(QString("Connected to broker %1 port %2").arg(host).arg(port));
    } else {
        displayMessage("Disconnected");
    }
}
