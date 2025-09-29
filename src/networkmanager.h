#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QMap>
#include <QQueue>
#include "message.h"

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject* parent = nullptr);
    ~NetworkManager();
    
    bool startServer(int port);
    void connectToNeighbor(const QString& host, int port);
    void sendMessage(const Message& message);
    
    void setNodeId(const QString& nodeId) { this->nodeId = nodeId; }
    QString getNodeId() const { return nodeId; }
    
    void addPeer(const QString& peerId, int port);
    void setRingTopology(const QList<int>& ports, int currentPort);

signals:
    void messageReceived(const Message& message);
    void connectionEstablished();
    void connectionLost();

private slots:
    void onNewConnection();
    void onDataReceived();
    void onDisconnected();
    void onConnectionError();
    void retryConnection();

private:
    void forwardMessage(const Message& message);
    void deliverMessage(const Message& message);
    void processReceivedData(QTcpSocket* socket);
    QString getNextHopForDestination(const QString& destination);
    
    QTcpServer* server;
    QTcpSocket* neighborSocket;
    QMap<QTcpSocket*, QByteArray> socketBuffers;
    QTimer* retryTimer;
    
    QString nodeId;
    int serverPort;
    QString neighborHost;
    int neighborPort;
    
    QMap<QString, int> peerPorts;
    QList<int> ringPorts;
    int currentPortIndex;
    
    QMap<QString, int> nextSequenceNumbers; // destination -> next sequence number
    QMap<QString, int> lastSequenceNumbers;
    QQueue<Message> messageQueue;
    
    // Sequence ordering mechanism
    QMap<QString, QMap<int, Message>> pendingMessages; // origin -> sequence -> message
    QMap<QString, int> expectedSequenceNumbers; // origin -> next expected sequence
    
    void processOrderedMessage(const Message& message);
    void deliverPendingMessages(const QString& origin);
    bool isSequenceExpected(const Message& message) const;
};