//
// Created by michael_uman on 5/14/23.
//

#ifndef MQTT_SERVICE_MQTTWRAPPER_H
#define MQTT_SERVICE_MQTTWRAPPER_H

#include <QObject>
#include <QScopedPointer>
#include <QByteArray>
#include <mosquitto.h>

constexpr auto DEFAULT_ID = "mqttwrapper";

/**
 * @class mqttWrapper
 * @brief Simple Qt wrapper on the mosquitto MQTT client core
 */
class mqttWrapper : public QObject {
Q_OBJECT
public:
    /**
     * Construct a mqttWrapper using id for client id.
     * @param id
     */
    explicit mqttWrapper(QString id = DEFAULT_ID);

    virtual ~mqttWrapper();

    Q_PROPERTY(bool connected READ getConnected NOTIFY onConnectionChanged)
    Q_PROPERTY(QString host READ getHost)

public slots:

    bool connect(const QString &host, int port = 1883);
    bool disconnect();

    bool subscribe(const QString &topic, int qos);
    bool unsubscribe(const QString &topic);

    bool publish(const QString &topic, const QByteArray &payload, int qos, bool retain);

signals:
    void onMessage(QString topic, QByteArray payload);
    void onConnectionChanged(bool connected);

private:

    bool getConnected();

    QString getHost();

    void log_callback(mosquitto *mosq, int level, const char *str);

    void connect_callback(mosquitto *mosq, int rc, int flags, const mosquitto_property *props);

    void disconnect_callback(mosquitto *mosq, int rc, const mosquitto_property *props);

    void publish_callback(mosquitto *mosq, int mid, int rc, const mosquitto_property *prop);

    void message_callback(mosquitto *mosq, const struct mosquitto_message *msg, const mosquitto_property *prop);

    void subscribe_callback(struct mosquitto *, int mid, int qos_count, const int *granted_qos,
                            const mosquitto_property *props);

    static void log_callback_(mosquitto *mosq, void *obj, int level, const char *str);

    static void
    connect_callback_(struct mosquitto *mosq, void *obj, int rc, int flags, const mosquitto_property *props);

    static void disconnect_callback_(struct mosquitto *mosq, void *obj, int rc, const mosquitto_property *props);

    static void publish_callback_(struct mosquitto *mosq, void *obj, int mid, int rc, const mosquitto_property *prop);

    static void message_callback_(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg,
                                  const mosquitto_property *prop);

    static void subscribe_callback_(struct mosquitto *, void *obj, int mid, int qos_count, const int *granted_qos,
                                    const mosquitto_property *props);

    QString conn_id;
    QString host;
    int port;

    mosquitto *mosq = nullptr;
    bool connected = false;

    QStringList topics;
};


#endif //MQTT_SERVICE_MQTTWRAPPER_H
