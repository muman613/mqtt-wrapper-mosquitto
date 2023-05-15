/**
 * @file mqttWrapper.cpp
 * @author Michael Uman <muman613@gmail.com>
 * @date May 14, 2023
 */

#include <QDebug>
#include <functional>
#include <utility>
#include "mqttWrapper.h"

using namespace std::placeholders;

static void displayMosquittoVersion() {
    int maj, min, rev;
    qDebug() << mosquitto_lib_version(&maj, &min, &rev);
    qDebug().nospace() << "Version : " << maj << '.' << min << '.' << rev;
}

mqttWrapper::mqttWrapper(QString id) : conn_id(std::move(id)) {
    qDebug() << Q_FUNC_INFO;
    mosquitto_lib_init();
    displayMosquittoVersion();

    mosq = mosquitto_new(conn_id.toStdString().c_str(), true, this);
    qDebug() << errno;

//    auto cb = std::bind(&mqttWrapper::log_callback_, this, _1, _2, _3, 4);

    mosquitto_log_callback_set(mosq, log_callback_);

    mosquitto_connect_v5_callback_set(mosq, connect_callback_);
    mosquitto_disconnect_v5_callback_set(mosq, disconnect_callback_);
    mosquitto_publish_v5_callback_set(mosq, publish_callback_);
    mosquitto_subscribe_v5_callback_set(mosq, subscribe_callback_);
    mosquitto_message_v5_callback_set(mosq, message_callback_);
}

mqttWrapper::~mqttWrapper() {
    qDebug() << Q_FUNC_INFO;
    if (mosq and connected) {
        qDebug() << "Disconnecting...";
        disconnect();
    }
    mosquitto_destroy(mosq);
    mosq = nullptr;
    mosquitto_lib_cleanup();
}

bool mqttWrapper::connect(const QString &host, int port) {
    qDebug() << Q_FUNC_INFO;
    port = port;
    auto rc = mosquitto_connect_async(mosq, host.toStdString().c_str(), port, 60);
    if (rc == MOSQ_ERR_SUCCESS) {
        rc = mosquitto_loop_start(mosq);
        if (rc != MOSQ_ERR_SUCCESS) {
            qWarning() << "Event loop failed to start";
        } else {
            qInfo() << "Connection OK";
            emit onConnectionChanged(true);
            connected = true;
        }
    } else {
        qWarning() << "Unable to connect to broken :" << host << "port :" << port;
    }

    return connected;
}

bool mqttWrapper::disconnect() {
    qDebug() << Q_FUNC_INFO;

    if (mosq and connected) {
        qDebug() << "Disconnecting";
        auto rc = mosquitto_disconnect(mosq);
        if (rc == MOSQ_ERR_SUCCESS) {
            qDebug() << "Stopping mosquitto event loop";
            mosquitto_loop_stop(mosq, true);
            emit onConnectionChanged(false);
            connected = false;
        } else {
            qWarning() << "Unable to disconnect";
        }
    }

    return (connected == false);
}


bool mqttWrapper::getConnected() {
    return connected;
}

QString mqttWrapper::getHost() {
    return host;
}

bool mqttWrapper::subscribe(const QString &topic, int qos) {
    bool result = false;
    qDebug() << Q_FUNC_INFO;
    if (mosq and connected) {
        int mid = -1;
        mosquitto_subscribe(mosq, &mid, topic.toStdString().c_str(), qos);
        result = true;
    } else {
        qWarning() << "Not connected";
    }
    return result;
}

bool  mqttWrapper::unsubscribe(const QString &topic) {
    qDebug() << Q_FUNC_INFO;
    bool result = false;
    if (mosq and connected) {
        int mid = -1;
        auto rc =mosquitto_unsubscribe(mosq, &mid, topic.toStdString().c_str());
        if (rc == MOSQ_ERR_SUCCESS) {
            result = true;
        } else {
            qWarning() << "Unable to unsubscribe";
        }
    }

    return result;
}


bool mqttWrapper::publish(const QString &topic, const QByteArray &payload, int qos, bool retain) {
    bool result = false;

    if (mosq and connected) {
        int mid = -1;
        auto rc = mosquitto_publish(mosq, &mid, topic.toStdString().c_str(), payload.length(), payload.data(), qos,
                                    retain);
        if (rc == MOSQ_ERR_SUCCESS) {
            qInfo() << "Published message #" << mid << "to topic" << topic;
            result = true;
        } else {
            qWarning() << "error" << strerror(errno);
        }
    } else {
        qWarning() << "Not connected";
    }
    return result;
}


void mqttWrapper::log_callback(mosquitto *mosq, int level, const char *str) {
    qDebug() << Q_FUNC_INFO << level << ':' << str;
    return;
}


void mqttWrapper::connect_callback(mosquitto *mosq, int rc, int flags, const mosquitto_property *props) {
    qDebug() << Q_FUNC_INFO;

}

void mqttWrapper::disconnect_callback(mosquitto *mosq, int rc, const mosquitto_property *props) {
    qDebug() << Q_FUNC_INFO;

}


void mqttWrapper::publish_callback(mosquitto *mosq, int mid, int rc, const mosquitto_property *prop) {
    qDebug() << Q_FUNC_INFO;

}

void
mqttWrapper::message_callback(mosquitto *mosq, const struct mosquitto_message *msg, const mosquitto_property *prop) {
    QByteArray message_array((char *) msg->payload, msg->payloadlen);
    QString topic = QString::fromLocal8Bit(msg->topic);

    emit onMessage(topic, message_array);

    QString message = QString::fromLocal8Bit(message_array);
    qDebug() << Q_FUNC_INFO << "topic :" << topic << "message :" << message;
}


void mqttWrapper::subscribe_callback(struct mosquitto *, int mid, int qos_count, const int *granted_qos,
                                     const mosquitto_property *props) {
    qDebug() << Q_FUNC_INFO;
}


void mqttWrapper::log_callback_(struct mosquitto *mosq, void *obj, int level, const char *str) {
    ((mqttWrapper *) obj)->log_callback(mosq, level, str);
}

void
mqttWrapper::connect_callback_(struct mosquitto *mosq, void *obj, int rc, int flags, const mosquitto_property *props) {
    ((mqttWrapper *) obj)->connect_callback(mosq, rc, flags, props);
}

void mqttWrapper::disconnect_callback_(struct mosquitto *mosq, void *obj, int rc, const mosquitto_property *props) {
    ((mqttWrapper *) obj)->disconnect_callback(mosq, rc, props);
}

void
mqttWrapper::publish_callback_(struct mosquitto *mosq, void *obj, int mid, int rc, const mosquitto_property *prop) {
    ((mqttWrapper *) obj)->publish_callback(mosq, mid, rc, prop);
}

void mqttWrapper::message_callback_(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg,
                                    const mosquitto_property *prop) {
    ((mqttWrapper *) obj)->message_callback(mosq, msg, prop);
}

void mqttWrapper::subscribe_callback_(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos,
                                      const mosquitto_property *props) {
    ((mqttWrapper *) obj)->subscribe_callback(mosq, mid, qos_count, granted_qos, props);

}
