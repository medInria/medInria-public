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
#include <teeStream.tpp>

class medLoggerPrivate;

class medLogger : public QObject
{
    Q_OBJECT

signals:
    void newQtMessage(QtMsgType type, const QString& message);

public:

    static medLogger& instance();

    static void initialize();
    static void finalize();

    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message);

private slots:
    void redirectQtMessage(QtMsgType type, const QString& message);
    void redirectMessage(const QString& message);
    void redirectErrorMessage(const QString& message);

private:
    medLoggerPrivate* const d;

    medLogger();
    ~medLogger();

    void initializeTeeStreams();
    void finalizeTeeStreams();

    void createTeeStream(std::ostream* targetStream);

    /** Test the size of the log file and cut if needed
     */
    void truncateLogFileIfHeavy();
};
