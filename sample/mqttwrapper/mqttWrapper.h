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
    Q_PROPERTY(int port READ getPort)

public slots:
    /**
     * Connect to a remote MQTT Broker.
     *
     * @param host Broker host address
     * @param port Broker port (default 1883)
     * @return true on success, false on failure.
     */
    bool connect(const QString &host, int port = 1883);
    /**
     * Disconnect from a connected broker.
     *
     * @return true on success, false on failure.
     */
    bool disconnect();
    /**
     * Subscribe to a topic. When a message is received it will be emitted via the onMessage signal.
     *
     * @param topic Topic to subscribe to
     * @param qos Quality of Service value to publish
     * @return true on success, false on failure.
     */
    bool subscribe(const QString &topic, int qos);
    /**
     * Unsubscribe from a subscribed topic.
     *
     * @param topic Topic to unsubscribe from
     * @return true on success, false on failure.
     */
    bool unsubscribe(const QString &topic);
    /**
     * Publish a message to a topic.
     *
     * @param topic Topic to publish to.
     * @param payload Payload message to publish.
     * @param qos Quality of Service for message
     * @param retain If true, retain this message.
     * @return true on success, false on failure.
     */
    bool publish(const QString &topic, const QByteArray &payload, int qos, bool retain);

signals:
    /**
     * Emitted when a message has been received.
     * @param topic Topic for message.
     * @param payload Payload for message.
     */
    void onMessage(QString topic, QByteArray payload);
    /**
     * Emitted when the connection state (connected/disconnected) changes.
     * @param connected True if connected, False if disconnected.
     */
    void onConnectionChanged(bool connected);

private:

    bool getConnected();

    QString getHost();

    int getPort();

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

    QString conn_id_;       // Connection ID
    QString broker_host_;   // Broker host address
    int broker_port_;       // Broker port #

    mosquitto *mosq = nullptr;
    bool connected = false;

    QStringList topics;
};


#endif //MQTT_SERVICE_MQTTWRAPPER_H
