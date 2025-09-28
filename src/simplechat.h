#pragma once

#include <QObject>
#include <QTimer>
#include "chatwindow.h"
#include "networkmanager.h"
#include "message.h"

class SimpleChat : public QObject {
    Q_OBJECT

public:
    explicit SimpleChat(int port, QObject* parent = nullptr);
    ~SimpleChat();
    
    void show();
    void setDestinationNode(const QString& destination);

private slots:
    void onMessageEntered(const QString& text, const QString& destination);
    void onMessageReceived(const Message& message);
    void onConnectionEstablished();
    void onConnectionLost();

private:
    void setupRingTopology();
    QString generateNodeId(int port);
    
    ChatWindow* window;
    NetworkManager* networkManager;
    int serverPort;
    QString nodeId;
    QString destinationNode;
    
    static const QList<int> RING_PORTS;
};