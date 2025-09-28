#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "simplechat.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QApplication::setApplicationName("SimpleChat");
    QApplication::setApplicationVersion("1.0");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("SimpleChat - Ring Network Messaging Application");
    parser.addHelpOption();
    parser.addVersionOption();
    
    QCommandLineOption portOption(QStringList() << "p" << "port",
                                  "Port number for this node (9001-9004)", "port", "9001");
    parser.addOption(portOption);
    
    parser.process(app);
    
    bool ok;
    int port = parser.value(portOption).toInt(&ok);
    
    if (!ok || port < 1024 || port > 65535) {
        qDebug() << "Invalid port number. Using default port 9001.";
        port = 9001;
    }
    
    SimpleChat chat(port);
    chat.show();
    
    return app.exec();
}