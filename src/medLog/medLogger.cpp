#include "medLogger.h"

#include <iostream>

medLogger* medLogger::singleton;
bool medLogger::logAccessFlag = false;

medLogger& medLogger::instance()
{
    return *singleton;
}

void medLogger::initialize()
{
    singleton = new medLogger();
    logAccessFlag = true;
}

void medLogger::finalize()
{
    logAccessFlag = false;
    singleton->deleteLater();
    singleton = NULL;
}

void medLogger::qtMessageHandler(QtMsgType type, const char* message)
{
    emit singleton->newQtMessage(type, QString(message));
}

void medLogger::redirectQtMessage(QtMsgType type, const QString& message)
{
    if (logAccessFlag)
    {
        logAccessFlag = false;

        switch (type)
        {
        case QtDebugMsg:
            dtkDebug() << message;
            break;
        case QtWarningMsg:
            dtkWarn() << message;
            break;
        case QtCriticalMsg:
            dtkError() << message;
            break;
        case QtFatalMsg:
            dtkFatal() << message;
            abort();
        }

        logAccessFlag = true;
    }
}

void medLogger::redirectMessage(const QString& message)
{
    dtkTrace() << message;
}

void medLogger::redirectErrorMessage(const QString& message)
{
    dtkError() << message;
}

medLogger::medLogger()
    : logFile(dtkLogPath(qApp).toLocal8Bit().data(), std::ios::app)
{
    qRegisterMetaType<QtMsgType>("QtMsgType");

    initializeTeeStreams();

    dtkLogger::instance().setLevel(logLevel);
    dtkLogger::instance().attachFile(dtkLogPath(qApp));
    dtkLogger::instance().attachConsole();

    QObject::connect(this, SIGNAL(newQtMessage(QtMsgType,QString)), this, SLOT(redirectQtMessage(QtMsgType,QString)));

    qInstallMsgHandler(qtMessageHandler);
}

medLogger::~medLogger()
{
    QObject::disconnect(this, SIGNAL(newQtMessage(QtMsgType,QString)), this, SLOT(redirectQtMessage(QtMsgType,QString)));
    qInstallMsgHandler(NULL);
    finalizeTeeStreams();
}

void medLogger::initializeTeeStreams()
{
    createTeeStream(&std::cout);
    createTeeStream(&std::cerr);
}

void medLogger::finalizeTeeStreams()
{
    for (int i = 0; i < redirectedStreams.length(); i++)
    {
        teeStreams.first()->flush();
        teeStreams.first()->close();
        delete teeStreams.takeFirst();
        delete redirectedStreamDummies.takeFirst();
        redirectedStreams.takeFirst()->rdbuf(previousStreamBuffers.takeFirst());
    }
}

void medLogger::createTeeStream(std::ostream* targetStream)
{
    redirectedStreams.append(targetStream);
    previousStreamBuffers.append(targetStream->rdbuf());
    redirectedStreamDummies.append(new std::ostream(targetStream->rdbuf()));
    TeeDevice* teeDevice = new TeeDevice(*redirectedStreamDummies.last(), logFile);
    teeStreams.append(new TeeStream(*teeDevice));
    targetStream->rdbuf(teeStreams.last()->rdbuf());
}

