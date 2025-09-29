#include "message.h"

Message::Message() : sequenceNumber(0) {}

Message::Message(const QString& chatText, const QString& origin, const QString& destination, int sequenceNumber)
    : chatText(chatText), origin(origin), destination(destination), sequenceNumber(sequenceNumber) {}

Message Message::fromVariantMap(const QVariantMap& map) {
    Message msg;
    msg.chatText = map.value("ChatText").toString();
    msg.origin = map.value("Origin").toString();
    msg.destination = map.value("Destination").toString();
    msg.sequenceNumber = map.value("SequenceNumber").toInt();
    return msg;
}

QVariantMap Message::toVariantMap() const {
    QVariantMap map;
    map["ChatText"] = chatText;
    map["Origin"] = origin;
    map["Destination"] = destination;
    map["SequenceNumber"] = sequenceNumber;
    return map;
}

bool Message::isValid() const {
    return !chatText.isEmpty() && !origin.isEmpty() && !destination.isEmpty() && sequenceNumber >= 1;
}

QDataStream& operator<<(QDataStream& stream, const Message& message) {
    QVariantMap map = message.toVariantMap();
    stream << map;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, Message& message) {
    QVariantMap map;
    stream >> map;
    message = Message::fromVariantMap(map);
    return stream;
}