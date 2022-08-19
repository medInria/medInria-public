/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medNewLogger.h"

#include <QtCore>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>

#include <iostream>
#include <fstream>
#include <string>

#include <teeStream.tpp>
#include <medNotification.h>

#if defined(WIN32)
#include <windows.h>

std::wstring convertUTF8toLocalUtfString(char const * pi_pchStringToConvert)
{
    std::wstring wsRes;

    WCHAR * pwcTmp = nullptr;

    int iWCharBuffSize = MultiByteToWideChar(CP_UTF8, 0, pi_pchStringToConvert, -1, nullptr, 0);
    if (iWCharBuffSize > 0)
    {
        pwcTmp = new WCHAR[iWCharBuffSize + 1];
        if (MultiByteToWideChar(CP_UTF8, 0, pi_pchStringToConvert, -1, pwcTmp, iWCharBuffSize + 1) == iWCharBuffSize)
        {
            wsRes = pwcTmp;
        }
        delete[] pwcTmp;
        pwcTmp = nullptr;
    }

    return wsRes;
}
#else
std::string convertUTF8toLocalUtfString(char const * pi_pchStringToConvert)
{
    return std::string(pi_pchStringToConvert);
}
#endif

class medNewLoggerPrivate
{
public:
    static medNewLogger* singleton;
    static bool initialized;
    bool logAccessFlag;

    //std::ofstream logFile;


    std::ofstream * mainLogFile;
    QMap<QString, std::ofstream *> logFilesMap;
    QMap<medLog *, streamProperty> streamCombinationMap;
    QMap<QtMsgType, streamProperty> qtmsgtypeCombinationMap;
        


    QList<std::ostream*> redirectedStreams;
    QList<std::ostream*> redirectedStreamDummies;
    QList<teestream*> teeStreams;
    QList<std::streambuf*> previousStreamBuffers;

    static const qint64 maxLogSize = 5*1024*1024;
    static const qint64 minLogSize = 1*1024*1024;

};

medNewLogger* medNewLoggerPrivate::singleton = nullptr;
bool medNewLoggerPrivate::initialized = false;


medLog medNewLogger::medLogWarning;   //std::stringstream medNewLogger::medLogDebug;
medLog medNewLogger::medLogDebug;     //std::stringstream medNewLogger::medLogWarning;
medLog medNewLogger::medLogCritical;  //std::stringstream medNewLogger::medLogCritical;
medLog medNewLogger::medLogFatal;     //std::stringstream medNewLogger::medLogFatal;
medLog medNewLogger::medLogInfo;      //std::stringstream medNewLogger::medLogInfo;



// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// medLogStream
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medLogStream::~medLogStream()
{
    m_pLog->write(m_string);
}



// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// medLog
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medLog::medLog(medNewLogger * logger)
{
    if (logger == nullptr)
    {
        m_logger = &medNewLogger::instance();
    }
    else
    {
        m_logger = logger;
    }

    m_logger->registerMedStream(*this);
}

medLog::~medLog()
{
    //TODO m_logger->unRegisterMedStream(*this);
}

medLogStream medLog::operator()()
{
    return medLogStream(this);
}

medLogStream medLog::operator()(streamProperty propoerty)
{
    // TODO
    return medLogStream(this);
}

void medLog::setProperty(streamProperty propoerty)
{
    m_logger->registerMedStream(*this, propoerty);
}

void medLog::write(QString data)
{
    m_accessMutex.lock();
    m_logger->writeMsgfromStream(this, data);
    m_accessMutex.unlock();
}



// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// medNewLogger
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medNewLogger::medNewLogger() : d(new medNewLoggerPrivate)
{
    qRegisterMetaType<QtMsgType>("QtMsgType");

    d->qtmsgtypeCombinationMap[QtDebugMsg];
    d->qtmsgtypeCombinationMap[QtWarningMsg];
    d->qtmsgtypeCombinationMap[QtCriticalMsg];
    d->qtmsgtypeCombinationMap[QtFatalMsg];
    d->qtmsgtypeCombinationMap[QtInfoMsg];
    d->mainLogFile = nullptr;

    truncateLogFileIfHeavy();
}

medNewLogger::~medNewLogger()
{
    qInstallMessageHandler(nullptr);
    if (medNewLoggerPrivate::singleton == this)
    {
        finalizeTeeStreams();
    }
}

medNewLogger& medNewLogger::instance()
{
    if (medNewLoggerPrivate::singleton == nullptr)
    {
        medNewLoggerPrivate::singleton = new medNewLogger();
    }
    return *medNewLoggerPrivate::singleton;
}

bool medNewLogger::initialize(medNewLogger * instance)
{
    bool bRes = false;

    if (!medNewLoggerPrivate::initialized && (medNewLoggerPrivate::singleton == nullptr || instance == nullptr))
    {
        if (medNewLoggerPrivate::singleton == nullptr)
        {
            if (instance == nullptr)
            {
                medNewLoggerPrivate::singleton = new medNewLogger();
            }
            else
            {
                medNewLoggerPrivate::singleton = instance;
            }
        }


        QString mainLogFileName = QString("%1.log").arg(qApp->applicationName());
        QString path = QDir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first()).filePath(mainLogFileName);
        
        medNewLoggerPrivate::singleton->d->logFilesMap[mainLogFileName] = new std::ofstream();
        medNewLoggerPrivate::singleton->d->logFilesMap[mainLogFileName]->open(convertUTF8toLocalUtfString(path.toStdString().c_str()), std::ios::app);
        medNewLoggerPrivate::singleton->d->mainLogFile = medNewLoggerPrivate::singleton->d->logFilesMap[mainLogFileName];

        medNewLoggerPrivate::singleton->d->logAccessFlag = true;
        medNewLoggerPrivate::singleton->initializeTeeStreams();
        // Redirect Qt messages
        qInstallMessageHandler(qtMessageHandler);
        medNewLoggerPrivate::initialized = true;
        bRes = true;
    }

    return bRes;
}

void medNewLogger::finalize()
{
    medNewLoggerPrivate::singleton->deleteLater();
    medNewLoggerPrivate::singleton = nullptr;
}

void medNewLogger::qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    medNewLoggerPrivate::singleton->redirectQtMessage(type, context, QString(message));
}

void medNewLogger::writeNotification(QtMsgType type, const QString & message)
{
    medNewLogger::instance().writeMsg(type, QString(), message, false);
}

void medNewLogger::redirectQtMessage(QtMsgType type, const QMessageLogContext &context, const QString& message)
{
    QString sContext;
    bool bNoContext = context.file == nullptr && context.function == nullptr && context.line > 0;    
    bNoContext ? sContext="" : sContext = QString(context.file) + QString(':') + QString(context.function) + QString(':') + QString::number(context.line);    
    
    writeMsg(type, sContext, message, true);
}

void medNewLogger::writeMsg(QtMsgType type, QString &sContext, const QString & message, bool bNotif)
{
    auto ctx = sContext.toUtf8().toStdString();
    auto msg = message.toUtf8().toStdString();
    if (d->logAccessFlag)
    {
        d->logAccessFlag = false;

        writeNewMsg(type, message);
        //switch (type)
        //{
        //    case QtInfoMsg:
        //    {
        //        std::cout << ctx << msg << std::endl;
        //        break;
        //    }
        //    case QtDebugMsg:
        //    {
        //        std::cout << ctx << msg << std::endl;
        //        if (bNotif)
        //        {
        //            medNotification::notify(medNotification::notifLevel::warnning, "logger", message);
        //        }
        //        break;
        //    }
        //    case QtWarningMsg:
        //    {
        //        std::cout << ctx << msg << std::endl;
        //        if (bNotif)
        //        {
        //            medNotification::notify(medNotification::notifLevel::warnning, "logger", message);
        //        }
        //        break;
        //    }
        //    case QtCriticalMsg:
        //    {
        //        std::cout << ctx << msg << std::endl;
        //        break;
        //    }
        //    case QtFatalMsg:
        //    {
        //        std::cout << ctx << msg << std::endl;
        //        abort();
        //    }
        //}

        d->logAccessFlag = true;
    }
}

void medNewLogger::writeNewMsg(QtMsgType type, const QString & message)
{
    if (d->qtmsgtypeCombinationMap.contains(type))
    {
        auto & prop = d->qtmsgtypeCombinationMap[type];
        auto msg = message.toUtf8().toStdString();
        if (prop.file)
        {
            if (prop.fileName.isEmpty())
            {
                if (d->mainLogFile)
                {
                    (*d->mainLogFile) << msg << std::endl;
                }
            }
            else
            {
                if (d->logFilesMap.contains(prop.fileName))
                {
                    (*d->logFilesMap[prop.fileName]) << msg << std::endl;
                }
            }
        }
        if (prop.coutConsole)
        {
            fprintf(stdout, "%s\n", msg.c_str());
        }
        if (prop.cerrConsole)
        {
            fprintf(stderr, "%s\n", msg.c_str());
        }
#ifdef _MSC_VER 
        if (prop.dbgConsole)
        {
            //TODO
        }
#endif // _MSC_VER 
    }    
}

void medNewLogger::initializeTeeStreams()
{
    createTeeStream(&std::cout);
    createTeeStream(&std::cerr);
    //createTeeStream(&medNewLogger::medLogDebug);
    //createTeeStream(&medNewLogger::medLogWarning);
    //createTeeStream(&medNewLogger::medLogCritical);
    //createTeeStream(&medNewLogger::medLogFatal);
    //createTeeStream(&medNewLogger::medLogInfo);
    //createTeeStream(&d->stream);
}

void medNewLogger::finalizeTeeStreams()
{
    for (int i = 0; i < d->redirectedStreams.length(); i++)
    {
        d->teeStreams.first()->flush();
        delete d->teeStreams.takeFirst();
        delete d->redirectedStreamDummies.takeFirst();
        d->redirectedStreams.takeFirst()->rdbuf(d->previousStreamBuffers.takeFirst());
    }
}

void medNewLogger::createTeeStream(std::ostream* targetStream)
{
    d->redirectedStreams.append(targetStream);

    d->previousStreamBuffers.append(targetStream->rdbuf());
    d->redirectedStreamDummies.append(new std::ostream(targetStream->rdbuf()));
    teestream* teeDevice = new teestream(*d->redirectedStreamDummies.last(), *d->mainLogFile);
    d->teeStreams.append(teeDevice);
    targetStream->rdbuf(d->teeStreams.last()->rdbuf());
}

void medNewLogger::truncateLogFileIfHeavy()
{
    //qint64 filesize = QFileInfo(d->path).size();
    //
    //// Over 5Mo, the file is truncated from the beginning (old lines are discarded)
    //if (filesize > d->maxLogSize)
    //{
    //
    //    QFile inFile(d->path);
    //    inFile.open(QFile::ReadOnly);
    //    inFile.seek(filesize - d->minLogSize); // file is going to be cut to minLogSize size
    //
    //    QTextStream keptText;
    //    while (!inFile.atEnd())
    //    {
    //        keptText << inFile.readLine();
    //        keptText << "\n";
    //    }
    //
    //    inFile.remove();
    //    inFile.close();
    //
    //    QFile outFile(d->path);
    //    outFile.open(QFile::ReadWrite | QFile::Truncate);
    //    outFile.write(keptText.string()->toUtf8());
    //
    //    outFile.close();
    //}
}


void medNewLogger::registerMedStream(medLog & log, streamProperty property)
{
    auto &map = medNewLoggerPrivate::singleton->d->streamCombinationMap;
    if (map.contains(&log))
    {

    }
    else
    {
        map[&log] = property;
    }

}

void medNewLogger::writeMsgfromStream(medLog * log, QString &data)
{

}
