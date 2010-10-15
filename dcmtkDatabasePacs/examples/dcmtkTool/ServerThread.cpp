#include "ServerThread.h"

#include "dcmtkStoreScp.h"
#include "dcmtkLogger.h"

ServerThread::ServerThread()
{
    m_server = new dcmtkStoreScp;
}

ServerThread::~ServerThread()
{
    delete m_server;
}

void ServerThread::run()
{
    dcmtkLogger::infoStream() << "Server listening on port: " << m_ourPort;
    m_server->start();
}

void ServerThread::setConnectionParams(const char* ourTitle, const char* ourIP, unsigned short ourPort)
{
    m_ourPort = ourPort;
    m_server->setConnectionParams(ourTitle, ourIP, ourPort);
}

void ServerThread::stop()
{
    m_server->stop();
}
