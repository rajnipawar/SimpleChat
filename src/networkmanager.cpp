#include "networkmanager.h"
#include <QDataStream>
#include <QHostAddress>
#include <QDebug>

NetworkManager::NetworkManager(QObject* parent) 
    : QObject(parent), server(nullptr), neighborSocket(nullptr), 
      serverPort(0), neighborPort(0), nextSequenceNumber(1), currentPortIndex(-1) {
    
    retryTimer = new QTimer(this);
    retryTimer->setSingleShot(true);
    connect(retryTimer, &QTimer::timeout, this, &NetworkManager::retryConnection);
}

NetworkManager::~NetworkManager() {
    if (neighborSocket) {
        neighborSocket->disconnectFromHost();
    }
    if (server) {
        server->close();
    }
}

bool NetworkManager::startServer(int port) {
    if (server) {
        server->close();
        delete server;
    }
    
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &NetworkManager::onNewConnection);
    
    if (!server->listen(QHostAddress::LocalHost, port)) {
        qDebug() << "Failed to start server on port" << port << ":" << server->errorString();
        return false;
    }
    
    serverPort = port;
    qDebug() << "Server started on port" << port;
    return true;
}

void NetworkManager::connectToNeighbor(const QString& host, int port) {
    neighborHost = host;
    neighborPort = port;
    
    if (neighborSocket) {
        neighborSocket->disconnectFromHost();
        neighborSocket->deleteLater();
    }
    
    neighborSocket = new QTcpSocket(this);
    connect(neighborSocket, &QTcpSocket::connected, this, &NetworkManager::connectionEstablished);
    connect(neighborSocket, &QTcpSocket::readyRead, this, &NetworkManager::onDataReceived);
    connect(neighborSocket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    connect(neighborSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkManager::onConnectionError);
    
    qDebug() << "Attempting to connect to neighbor at" << host << ":" << port;
    neighborSocket->connectToHost(host, port);
}

void NetworkManager::setRingTopology(const QList<int>& ports, int currentPort) {
    ringPorts = ports;
    currentPortIndex = ringPorts.indexOf(currentPort);
    
    if (currentPortIndex != -1 && !ringPorts.isEmpty()) {
        int nextIndex = (currentPortIndex + 1) % ringPorts.size();
        int nextPort = ringPorts[nextIndex];
        
        if (nextPort != currentPort) {
            // Add delay before connecting to allow all servers to start
            QTimer::singleShot(2000, this, [this, nextPort]() {
                connectToNeighbor("localhost", nextPort);
            });
        }
    }
}

void NetworkManager::sendMessage(const Message& message) {
    if (!message.isValid()) {
        qDebug() << "Invalid message, not sending";
        return;
    }
    
    Message msgToSend = message;
    msgToSend.setSequenceNumber(nextSequenceNumber++);
    
    if (msgToSend.getDestination() == nodeId) {
        deliverMessage(msgToSend);
    } else {
        forwardMessage(msgToSend);
    }
}

void NetworkManager::forwardMessage(const Message& message) {
    if (!neighborSocket || neighborSocket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "No connection to neighbor, queuing message. Socket state:" 
                 << (neighborSocket ? neighborSocket->state() : -1);
        messageQueue.enqueue(message);
        return;
    }
    
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << message;
    
    QByteArray sizeData;
    QDataStream sizeStream(&sizeData, QIODevice::WriteOnly);
    sizeStream << (quint32)data.size();
    
    neighborSocket->write(sizeData);
    neighborSocket->write(data);
    neighborSocket->flush();
    
    qDebug() << "Forwarded message from" << message.getOrigin() << "to" << message.getDestination() 
             << "via" << neighborHost << ":" << neighborPort;
}

void NetworkManager::deliverMessage(const Message& message) {
    emit messageReceived(message);
}

void NetworkManager::onNewConnection() {
    while (server->hasPendingConnections()) {
        QTcpSocket* clientSocket = server->nextPendingConnection();
        connect(clientSocket, &QTcpSocket::readyRead, this, &NetworkManager::onDataReceived);
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
        
        socketBuffers[clientSocket] = QByteArray();
        qDebug() << "New client connected from" << clientSocket->peerAddress().toString();
    }
}

void NetworkManager::onDataReceived() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    processReceivedData(socket);
}

void NetworkManager::processReceivedData(QTcpSocket* socket) {
    QByteArray& buffer = socketBuffers[socket];
    buffer.append(socket->readAll());
    
    while (buffer.size() >= sizeof(quint32)) {
        QDataStream sizeStream(buffer);
        quint32 messageSize;
        sizeStream >> messageSize;
        
        if (buffer.size() < sizeof(quint32) + messageSize) {
            break;
        }
        
        buffer.remove(0, sizeof(quint32));
        QByteArray messageData = buffer.left(messageSize);
        buffer.remove(0, messageSize);
        
        QDataStream messageStream(messageData);
        Message message;
        messageStream >> message;
        
        if (message.isValid()) {
            qDebug() << "Received message from" << message.getOrigin() << "to" << message.getDestination();
            
            if (message.getDestination() == nodeId) {
                deliverMessage(message);
            } else {
                forwardMessage(message);
            }
        }
    }
}

void NetworkManager::onDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        socketBuffers.remove(socket);
        
        if (socket == neighborSocket) {
            qDebug() << "Lost connection to neighbor, will retry";
            emit connectionLost();
            retryTimer->start(3000);
        }
    }
}

void NetworkManager::onConnectionError() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket == neighborSocket) {
        qDebug() << "Connection error:" << socket->errorString();
        retryTimer->start(3000);
    }
}

void NetworkManager::retryConnection() {
    if (!neighborHost.isEmpty() && neighborPort > 0) {
        qDebug() << "Retrying connection to neighbor" << neighborHost << ":" << neighborPort;
        connectToNeighbor(neighborHost, neighborPort);
    }
}

void NetworkManager::addPeer(const QString& peerId, int port) {
    peerPorts[peerId] = port;
}