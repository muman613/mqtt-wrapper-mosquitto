#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mqttWrapper.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_pubButton_clicked();

    void on_subButton_clicked();

    void on_unSubButton_clicked();

    void on_actionClear_message_text_triggered();

    void on_loadButton_clicked();

    void on_actionAbout_mqttgui_triggered();

    void on_connectButton_clicked();

    void onMessage(QString topic, QByteArray payload);
    void onConnectionChanged(bool connected);

private:
    Ui::MainWindow *ui;
    mqttWrapper mqtt;
    void enableInterface(bool enable);
    QString client_id = "client-mqttgui2";

    void restoreOptions();
    void saveOptions();

    void displayMessage(const QString &msg);
};
#endif // MAINWINDOW_H
