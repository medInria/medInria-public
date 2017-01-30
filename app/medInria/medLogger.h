/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2014. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <fstream>

#ifndef Q_MOC_RUN
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#endif

#include <dtkLog/dtkLog.h>

#include <QtGui>

class medLoggerPrivate;

class medLogger: public QObject
{
    Q_OBJECT

signals :
    void newQtMessage(QtMsgType type, const QString& message);

public :
    static const dtkLog::Level logLevel = dtkLog::Debug;

    static medLogger& instance();

    static void initialize();
    static void finalize();

    static void qtMessageHandler(QtMsgType type, const char* message);

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
};
