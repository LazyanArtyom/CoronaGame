#include "tcpserver.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CGameServer server;

    return a.exec();
}
