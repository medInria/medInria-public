/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medLogger.h"

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

class medLoggerPrivate
{
public:
    static medLogger* singleton;
    static bool logAccessFlag;

    QString path;
    std::ofstream logFile;
    QList<std::ostream*> redirectedStreams;
    QList<std::ostream*> redirectedStreamDummies;
    QList<teestream*> teeStreams;
    QList<std::streambuf*> previousStreamBuffers;

    static const qint64 maxLogSize = 5*1024*1024;
    static const qint64 minLogSize = 1*1024*1024;
    
    QString localBuff;
    QDebug stream;
    medLoggerPrivate() : stream(&localBuff)
    {
        path = QDir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first()).filePath(QString("%1.log").arg(qApp->applicationName()));

        logFile.open(convertUTF8toLocalUtfString(path.toStdString().c_str()), std::ios::app);
    }
};

medLogger* medLoggerPrivate::singleton = nullptr;
bool medLoggerPrivate::logAccessFlag = false;

std::stringstream medLogger::medLogDebug;
std::stringstream medLogger::medLogWarning;
std::stringstream medLogger::medLogCritical;
std::stringstream medLogger::medLogFatal;
std::stringstream medLogger::medLogInfo;


medLogger::medLogger() : d(new medLoggerPrivate)
{
    qRegisterMetaType<QtMsgType>("QtMsgType");

    initializeTeeStreams();

    truncateLogFileIfHeavy();

    // Redirect Qt messages
    QObject::connect(this, SIGNAL(newQtMessage(QtMsgType, QString)), this, SLOT(redirectQtMessage(QtMsgType, QString)));
    qInstallMessageHandler(qtMessageHandler);
}

medLogger::~medLogger()
{
    QObject::disconnect(this, SIGNAL(newQtMessage(QtMsgType, QString)), this, SLOT(redirectQtMessage(QtMsgType, QString)));
    qInstallMessageHandler(nullptr);
    finalizeTeeStreams();
}

medLogger& medLogger::instance()
{
    return *medLoggerPrivate::singleton;
}

void medLogger::initialize()
{
    medLoggerPrivate::singleton = new medLogger();
    medLoggerPrivate::logAccessFlag = true;
}

void medLogger::finalize()
{
    medLoggerPrivate::logAccessFlag = false;
    medLoggerPrivate::singleton->deleteLater();
    medLoggerPrivate::singleton = nullptr;
}

void medLogger::qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    emit medLoggerPrivate::singleton->newQtMessage(type, context, QString(message));
}

void medLogger::writeNotification(QtMsgType type, const QString & message)
{
    medLogger::instance().writeMsg(type, QString(), message, false);
}

void medLogger::redirectQtMessage(QtMsgType type, const QMessageLogContext &context, const QString& message)
{
    QString sContext;
    bool bNoContext = context.file == nullptr && context.function == nullptr && context.line > 0;    
    bNoContext ? sContext="" : sContext = QString(context.file) + QString(':') + QString(context.function) + QString(':') + QString::number(context.line);    
    
    writeMsg(type, sContext, message, true);
}

void medLogger::writeMsg(QtMsgType type, QString &sContext, const QString & message, bool bNotif)
{
    if (d->logAccessFlag)
    {
        d->logAccessFlag = false;

        switch (type)
        {
            case QtInfoMsg:
            {
                d->stream << sContext << message;
                break;
            }
            case QtDebugMsg:
            {
                d->stream << sContext << message;
                if (bNotif)
                {
                    medNotification::notify(medNotification::notifLevel::warnning, "logger", message);
                }
                break;
            }
            case QtWarningMsg:
            {
                d->stream << sContext << message;
                if (bNotif)
                {
                    medNotification::notify(medNotification::notifLevel::warnning, "logger", message);
                }
                break;
            }
            case QtCriticalMsg:
            {
                d->stream << sContext << message;
                break;
            }
            case QtFatalMsg:
            {
                d->stream << sContext << message;
                abort();
            }
        }

        d->logAccessFlag = true;
    }
}

void medLogger::redirectMessage(const QString& message)
{
    d->stream << message;
}

void medLogger::redirectErrorMessage(const QString& message)
{
    d->stream << message;
}

void medLogger::initializeTeeStreams()
{
    createTeeStream(&std::cout);
    createTeeStream(&std::cerr);
    createTeeStream(&medLogger::medLogDebug);
    createTeeStream(&medLogger::medLogWarning);
    createTeeStream(&medLogger::medLogCritical);
    createTeeStream(&medLogger::medLogFatal);
    createTeeStream(&medLogger::medLogInfo);
}

void medLogger::finalizeTeeStreams()
{
    for (int i = 0; i < d->redirectedStreams.length(); i++)
    {
        d->teeStreams.first()->flush();
        delete d->teeStreams.takeFirst();
        delete d->redirectedStreamDummies.takeFirst();
        d->redirectedStreams.takeFirst()->rdbuf(d->previousStreamBuffers.takeFirst());
    }
}

void medLogger::createTeeStream(std::ostream* targetStream)
{
    d->redirectedStreams.append(targetStream);

    d->previousStreamBuffers.append(targetStream->rdbuf());
    d->redirectedStreamDummies.append(new std::ostream(targetStream->rdbuf()));
    teestream* teeDevice = new teestream(*d->redirectedStreamDummies.last(), d->logFile);
    d->teeStreams.append(teeDevice);
    targetStream->rdbuf(d->teeStreams.last()->rdbuf());
}

void medLogger::truncateLogFileIfHeavy()
{
    qint64 filesize = QFileInfo(d->path).size();

    // Over 5Mo, the file is truncated from the beginning (old lines are discarded)
    if (filesize > d->maxLogSize)
    {

        QFile inFile(d->path);
        inFile.open(QFile::ReadOnly);
        inFile.seek(filesize - d->minLogSize); // file is going to be cut to minLogSize size

        QString keptText;
        while (!inFile.atEnd())
        {
            keptText += inFile.readLine(); //TODO redo with stringstream because it's very poor programing with string
            keptText += "\n";
        }

        inFile.remove();
        inFile.close();

        QFile outFile(d->path);
        outFile.open(QFile::ReadWrite | QFile::Truncate);
        outFile.write(keptText.toUtf8());

        outFile.close();
    }
}

