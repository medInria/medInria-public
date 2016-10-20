/*=========================================================================

medInria

Copyright (c) INRIA 2013 - 2014. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#include <fstream>
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>

#include <dtkLog/dtkLog.h>
#include <medLogExport.h>

#include <QtGui>

class MEDLOG_EXPORT medLogger: public QObject
{
    Q_OBJECT

    typedef boost::iostreams::tee_device<std::ostream, std::ofstream> TeeDevice;
    typedef boost::iostreams::stream<TeeDevice> TeeStream;

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
    static medLogger* singleton;
    static bool logAccessFlag;

    std::ofstream logFile;
    QList<std::ostream*> redirectedStreams;
    QList<std::ostream*> redirectedStreamDummies;
    QList<TeeStream*> teeStreams;
    QList<std::streambuf*> previousStreamBuffers;

    medLogger();
    ~medLogger();

    void initializeTeeStreams();
    void finalizeTeeStreams();

    void createTeeStream(std::ostream* targetStream);
};
