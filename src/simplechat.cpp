#include "simplechat.h"
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

const QList<int> SimpleChat::RING_PORTS = {9001, 9002, 9003, 9004};

SimpleChat::SimpleChat(int port, QObject* parent) 
    : QObject(parent), serverPort(port) {
    
    nodeId = generateNodeId(port);
    
    window = new ChatWindow();
    window->setNodeId(nodeId);
    
    networkManager = new NetworkManager(this);
    networkManager->setNodeId(nodeId);
    
    connect(window, &ChatWindow::messageEntered, this, &SimpleChat::onMessageEntered);
    connect(networkManager, &NetworkManager::messageReceived, this, &SimpleChat::onMessageReceived);
    connect(networkManager, &NetworkManager::connectionEstablished, this, &SimpleChat::onConnectionEstablished);
    connect(networkManager, &NetworkManager::connectionLost, this, &SimpleChat::onConnectionLost);
    
    if (!networkManager->startServer(port)) {
        QMessageBox::critical(nullptr, "Error", QString("Failed to start server on port %1").arg(port));
        QApplication::exit(1);
        return;
    }
    
    setupRingTopology();
    
    window->appendMessage(QString("SimpleChat Node %1 started on port %2").arg(nodeId).arg(port));
    window->appendMessage("Available nodes: Node1 (9001), Node2 (9002), Node3 (9003), Node4 (9004)");
    window->appendMessage("Select destination from dropdown and type your message");
    window->appendMessage("Messages will be routed through the ring network");
}

SimpleChat::~SimpleChat() {
    delete window;
}

void SimpleChat::show() {
    window->show();
}

QString SimpleChat::generateNodeId(int port) {
    int nodeNumber = RING_PORTS.indexOf(port) + 1;
    if (nodeNumber > 0) {
        return QString("Node%1").arg(nodeNumber);
    }
    return QString("Node%1").arg(port);
}

void SimpleChat::setupRingTopology() {
    networkManager->setRingTopology(RING_PORTS, serverPort);
}

void SimpleChat::onMessageEntered(const QString& text, const QString& destination) {
    QString trimmedText = text.trimmed();
    if (trimmedText.isEmpty()) {
        window->appendMessage("Message cannot be empty.");
        return;
    }
    
    if (destination.isEmpty()) {
        window->appendMessage("Please select a destination.");
        return;
    }
    
    // Create message with placeholder sequence number (NetworkManager will assign the correct one)
    Message message(trimmedText, nodeId, destination, 1);
    qDebug() << "Sending message from" << nodeId << "to" << destination << ":" << trimmedText;
    networkManager->sendMessage(message);
    
    // Add to conversation with destination node as sent message
    window->appendSentMessage(destination, trimmedText);
}

void SimpleChat::onMessageReceived(const Message& message) {
    // Add to conversation with sender node as received message
    window->appendReceivedMessage(message.getOrigin(), message.getChatText());
    qDebug() << "Message delivered from" << message.getOrigin() << ":" << message.getChatText();
}

void SimpleChat::onConnectionEstablished() {
    window->appendMessage("Connected to ring network");
}

void SimpleChat::onConnectionLost() {
    window->appendMessage("Lost connection to ring network, attempting to reconnect...");
}

void SimpleChat::setDestinationNode(const QString& destination) {
    destinationNode = destination;
}