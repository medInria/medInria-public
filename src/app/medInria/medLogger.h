/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2014. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <dtkLog/dtkLog.h>

#include <QtGui>

class medLoggerPrivate;

class medLogger: public QObject
{
    Q_OBJECT

signals :
    void newQtMessage(QtMsgType type, const QString& message);

public :
    // TRACE >> DEBUG >> INFO >> WARN >> ERROR >> FATAL
    // Note that these levels form a hierarchy.
    // If dtk’s logging level is set to TRACE, all logs will be visible,
    // unless set to WARN, only WARN, ERROR and FATAL log messages will be visible.
    static const dtkLog::Level logLevel = dtkLog::Debug;

    static medLogger& instance();

    static void initialize();
    static void finalize();

    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message);

private slots:
    void redirectQtMessage(QtMsgType type, const QString& message);

private:
    medLoggerPrivate* const d;

    medLogger();
    ~medLogger();

    /** Test the size of the log file and cut if needed
     */
    void truncateLogFileIfHeavy();
};
