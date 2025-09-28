#pragma once

#include <QVariantMap>
#include <QString>
#include <QDataStream>

class Message {
public:
    Message();
    Message(const QString& chatText, const QString& origin, const QString& destination, int sequenceNumber);
    
    static Message fromVariantMap(const QVariantMap& map);
    QVariantMap toVariantMap() const;
    
    QString getChatText() const { return chatText; }
    QString getOrigin() const { return origin; }
    QString getDestination() const { return destination; }
    int getSequenceNumber() const { return sequenceNumber; }
    
    void setChatText(const QString& text) { chatText = text; }
    void setOrigin(const QString& org) { origin = org; }
    void setDestination(const QString& dest) { destination = dest; }
    void setSequenceNumber(int seq) { sequenceNumber = seq; }
    
    bool isValid() const;
    
private:
    QString chatText;
    QString origin;
    QString destination;
    int sequenceNumber;
};

QDataStream& operator<<(QDataStream& stream, const Message& message);
QDataStream& operator>>(QDataStream& stream, Message& message);