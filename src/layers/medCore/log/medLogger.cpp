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

#if defined(WIN32)
#include <Stringapiset.h>

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

//typedef boost::iostreams::tee_device<std::ostream, std::ofstream> TeeDevice;
//typedef boost::iostreams::stream<TeeDevice> TeeStream;

class medLoggerPrivate
{
public:
    static medLogger* singleton;
    static bool logAccessFlag;

    std::ofstream logFile;
    QList<std::ostream*> redirectedStreams;
    QList<std::ostream*> redirectedStreamDummies;
    QList<TeeStream*> teeStreams;
    QList<std::streambuf*> previousStreamBuffers;

    static const qint64 maxLogSize = 5000000;
    static const qint64 minLogSize = 1000000;

    QDebug stream;
    medLoggerPrivate() : logFile(convertUTF8toLocalUtfString(dtkLogPath(qApp).toUtf8().data()), std::ios::app) {}
};

medLogger* medLoggerPrivate::singleton = nullptr;
bool medLoggerPrivate::logAccessFlag = false;

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
    Q_UNUSED(context);
    emit medLoggerPrivate::singleton->newQtMessage(type, QString(message));
}

void medLogger::redirectQtMessage(QtMsgType type, const QString& message)
{
    if (d->logAccessFlag)
    {
        d->logAccessFlag = false;

        switch (type)
        {
            case QtInfoMsg:
            {
                d->stream << message;
                break;
            }
            case QtDebugMsg:
            {
                d->stream << message;
                break;
            }
            case QtWarningMsg:
            {
                d->stream << message;
                break;
            }
            case QtCriticalMsg:
            {
                d->stream << message;
                break;
            }
            case QtFatalMsg:
            {
                d->stream << message;
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

medLogger::medLogger() : d(new medLoggerPrivate)
{
    qRegisterMetaType<QtMsgType>("QtMsgType");

    initializeTeeStreams();

    truncateLogFileIfHeavy();

    //dtkLogger::instance().setLevel(logLevel);

    dtkLogger::instance().attachFile(dtkLogPath(qApp));
    dtkLogger::instance().attachConsole();

    // Redirect Qt messages
    QObject::connect(this, SIGNAL(newQtMessage(QtMsgType, QString)), this, SLOT(redirectQtMessage(QtMsgType, QString)));
    qInstallMessageHandler(qtMessageHandler);
}

medLogger::~medLogger()
{
    dtkLogger::instance().detachFile(dtkLogPath(qApp));
    dtkLogger::instance().detachConsole();

    QObject::disconnect(this, SIGNAL(newQtMessage(QtMsgType, QString)), this, SLOT(redirectQtMessage(QtMsgType, QString)));
    qInstallMessageHandler(nullptr);
    finalizeTeeStreams();
}

void medLogger::initializeTeeStreams()
{
    createTeeStream(&std::cout);
    createTeeStream(&std::cerr);
}

void medLogger::finalizeTeeStreams()
{
    for (int i = 0; i < d->redirectedStreams.length(); i++)
    {
        d->teeStreams.first()->flush();
        d->teeStreams.first()->close();
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
    d->teeStreams.append(new teestream(*teeDevice));
    targetStream->rdbuf(d->teeStreams.last()->rdbuf());
}

void medLogger::truncateLogFileIfHeavy()
{
    qint64 filesize = QFileInfo(dtkLogPath(qApp)).size();

    // Over 5Mo, the file is truncated from the beginning (old lines are discarded)
    if (filesize > d->maxLogSize)
    {
        QString path = dtkLogPath(qApp);

        QFile inFile(path);
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

        QFile outFile(path);
        outFile.open(QFile::ReadWrite | QFile::Truncate);
        outFile.write(keptText.toUtf8());

        outFile.close();
    }
}
