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

#include <medLog.h>

class medNewLoggerPrivate;
class medNewLogger;

struct streamProperty
{
    inline streamProperty()
    {
        context = false;
        timestamp = true;

        file = true;
        coutConsole = true;
        cerrConsole = false;
        dbgConsole = true;
    }

    QString fileName;

    bool context;
    bool timestamp;

    bool file;
    bool coutConsole;
    bool cerrConsole;
    bool dbgConsole;
};

enum eLogLevel {medNoneMsg = -1, medDebugMsg = 0, medInfoMsg, medWarningMsg, medErrorMsg, medFatalMsg};

struct sLogLevel
{
    inline sLogLevel() : levelArray{ 1,1,1,1,1 }, debug(levelArray[medDebugMsg]), warning(levelArray[medInfoMsg]),
                         error(levelArray[medWarningMsg]), info(levelArray[medErrorMsg]), fatal(levelArray[medFatalMsg]) {}
    inline bool & operator[](eLogLevel level) { return levelArray[level]; }
    inline sLogLevel & operator= (sLogLevel const & other) { std::copy_n(other.levelArray, 5, levelArray); return *this; }

    bool levelArray[5];
    bool & debug;
    bool & info;
    bool & warning;
    bool & error;
    bool & fatal;
};




class MEDCORE_EXPORT medNewLogger : public QObject
{
    Q_OBJECT
public:
    static medLog medLogDebug;
    static medLog medLogWarning;
    static medLog medLogCritical;
    static medLog medLogFatal;
    static medLog medLogInfo;

public:
    medNewLogger();
    ~medNewLogger();

    static medNewLogger& mainInstance();

    static bool initialize(medNewLogger * instance = nullptr);
    static void finalize();

    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message);

    bool setLogPath(QString path);
    void setLogApp(QApplication * app);
    bool setLogOutputFileProperty(QString fileName, sLogLevel property);
    bool enableQtRedirection(bool enable);

    void registerMedStream(medLog &log, streamProperty property = streamProperty());
    void registerStdStream(std::ostream &log, streamProperty property = streamProperty());

    void createOutputFileIfNeeded(QString fileName);

    void writeMsgfromMedStream(medLog * log, QString &data, streamProperty *property = nullptr);
    void writeMsgfromStdStream(std::ostream * log, QString &data);

private:
    void redirectQtMessage(QtMsgType type, const QMessageLogContext &context, const QString& message);
    QString filePath(QString const & fileName);

private:
    medNewLoggerPrivate* const d;

    void writeMsgInternal(eLogLevel type, streamProperty const & prop, QString const & message);

    bool outputIsEnable(QString outputName, eLogLevel level);

    static eLogLevel getLevelOfMsg(QString const & msg);
    static eLogLevel getLevelOfQt(QtMsgType const type);    
    static void truncateLogFileIfHeavy(QString const & path);
};



#define mDebug    medNewLogger::medLogDebug()    << "[DBG] "
#define mInfo     medNewLogger::medLogInfo()     << "[INF] "
#define mWarning  medNewLogger::medLogWarning()  << "[WRN] "
#define mCritical medNewLogger::medLogCritical() << "[CRT] "
#define mFatal    medNewLogger::medLogFatal()    << "[FAT] "

