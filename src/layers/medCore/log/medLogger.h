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

#include <sstream>

class medLoggerPrivate;

class medLogger : public QObject
{
    Q_OBJECT
public:    
    static std::stringstream medLogDebug;
    static std::stringstream medLogWarning;
    static std::stringstream medLogCritical;
    static std::stringstream medLogFatal;
    static std::stringstream medLogInfo;

public:
    static medLogger& instance();

    static void initialize();
    static void finalize();

    static void writeNotification(QtMsgType type, const QString &message);
    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message);

signals:
    void newQtMessage(QtMsgType type, const QMessageLogContext &context, const QString& message);

private slots:
    void redirectQtMessage(QtMsgType type, const QMessageLogContext &context, const QString& message);
    void redirectMessage(const QString& message);
    void redirectErrorMessage(const QString& message);

private:
    medLoggerPrivate* const d;

    medLogger();
    ~medLogger();

    void initializeTeeStreams();
    void finalizeTeeStreams();

    void createTeeStream(std::ostream* targetStream);
    void writeMsg(QtMsgType type, QString &sContext, const QString & message, bool bNotif);

    /** Test the size of the log file and cut if needed
     */
    void truncateLogFileIfHeavy();
};

#define medDebug    medLogger::medLogDebug    << "[DBG] "
#define medWarning  medLogger::medLogWarning  << "[WRN] "
#define medCritical medLogger::medLogCritical << "[CRT] "
#define medFatal    medLogger::medLogFatal    << "[FAT] "
#define medInfo     medLogger::medLogInfo     << "[INF] "

