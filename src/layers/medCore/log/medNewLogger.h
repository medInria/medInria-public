#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2022. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>
#include <medCoreExport.h>
#include <sstream>
#include <QDebug>
#include <QBuffer>
#include <QMutex>

class medNewLoggerPrivate;
class medNewLogger;
class medLog;

struct streamProperty
{
    inline streamProperty()
    {
        debug = true;
        warning = true;
        error = true;
        info = true;
        fatal = true;

        context = false;
        timestamp = true;

        file = true;
        coutConsole = true;
        cerrConsole = false;
        dbgConsole = true;
    }
    QString fileName;

    bool debug;
    bool warning;
    bool error;
    bool info;
    bool fatal;

    bool context;
    bool timestamp;

    bool file;
    bool coutConsole;
    bool cerrConsole;
    bool dbgConsole;
};





class MEDCORE_EXPORT medLogStream : public QDebug
{
public:
    inline medLogStream(medLog *log) : QDebug(&m_string), m_pLog(log) { }
    //medLogStream(medLogStream && x);
    ~medLogStream();

private:
    medLog * m_pLog;
    QString m_string;
};





class MEDCORE_EXPORT medLog
{
public:
    medLog(medNewLogger * logger = nullptr);
    ~medLog();

    medLogStream operator()();
    medLogStream operator()(streamProperty propoerty);

    void write(QString data);
    void setProperty(streamProperty propoerty);

private:
    friend class medNewLogger;

private:
    QMutex m_accessMutex;
    medNewLogger * m_logger;
};





class MEDCORE_EXPORT medNewLogger : public QObject
{
    Q_OBJECT
public:
    static medLog medLogDebug;   //static std::stringstream medLogDebug;
    static medLog medLogWarning; //static std::stringstream medLogWarning;
    static medLog medLogCritical;//static std::stringstream medLogCritical;
    static medLog medLogFatal;   //static std::stringstream medLogFatal;
    static medLog medLogInfo;    //static std::stringstream medLogInfo;

public:
    medNewLogger();
    ~medNewLogger();

    static medNewLogger& instance();

    static bool initialize(medNewLogger * instance = nullptr);
    static void finalize();

    static void writeNotification(QtMsgType type, const QString &message);
    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message);

    static void registerMedStream(medLog &log, streamProperty property = streamProperty() );

    friend void medLog::write(QString data);

private:
    void redirectQtMessage(QtMsgType type, const QMessageLogContext &context, const QString& message);

private:
    medNewLoggerPrivate* const d;

    

    void initializeTeeStreams();
    void finalizeTeeStreams();

    void createTeeStream(std::ostream* targetStream);
    void writeMsg(QtMsgType type, QString &sContext, const QString & message, bool bNotif);
    void writeNewMsg(QtMsgType type, const QString & message);
    void writeMsgfromStream(medLog * log, QString &data);

    /** Test the size of the log file and cut if needed
     */
    void truncateLogFileIfHeavy();
};



#define mDebug    medNewLogger::medLogDebug()    << "[DBG] "
#define mWarning  medNewLogger::medLogWarning()  << "[WRN] "
#define mCritical medNewLogger::medLogCritical() << "[CRT] "
#define mFatal    medNewLogger::medLogFatal()    << "[FAT] "
#define mInfo     medNewLogger::medLogInfo()     << "[INF] "

