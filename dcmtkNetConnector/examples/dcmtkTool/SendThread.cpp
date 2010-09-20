#include "SendThread.h"

#include "dcmtkStoreScu.h"
#include "dcmtkLogger.h"

SendThread::SendThread()
{
    m_storeScu = new dcmtkStoreScu;
}

SendThread::~SendThread()
{
    delete m_storeScu;
}

void SendThread::run()
{
    dcmtkLogger::infoStream() << "thread start store-scu";
    m_storeScu->sendStoreRequest();
}

void SendThread::setConnectionParams(const char* peerTitle, const char* peerIP, int peerPort, 
                                     const char* ourTitle, const char* ourIP, int ourPort)
{
    m_storeScu->setConnectionParams(peerTitle, peerIP, peerPort, ourTitle, ourIP, ourPort);
}

void SendThread::setScanDirectory(const char* dir)
{
    m_storeScu->setScanDirectory(dir);
}
