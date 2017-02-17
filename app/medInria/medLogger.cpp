#include "medLogger.h"

#include <iostream>

typedef boost::iostreams::tee_device<std::ostream, std::ofstream> TeeDevice;
typedef boost::iostreams::stream<TeeDevice> TeeStream;

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

    medLoggerPrivate() : logFile(dtkLogPath(qApp).toLocal8Bit().data(), std::ios::app) {}
};

medLogger* medLoggerPrivate::singleton = NULL;
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
    medLoggerPrivate::singleton = NULL;
}

void medLogger::qtMessageHandler(QtMsgType type, const char* message)
{
    emit medLoggerPrivate::singleton->newQtMessage(type, QString(message));
}

void medLogger::redirectQtMessage(QtMsgType type, const QString& message)
{
    if (d->logAccessFlag)
    {
        d->logAccessFlag = false;

        switch (type)
        {
        case QtDebugMsg:
            dtkDebug() << message;
            break;
        case QtWarningMsg:
            dtkWarn() << message;
            break;
        case QtCriticalMsg:
            dtkError() << message;
            break;
        case QtFatalMsg:
            dtkFatal() << message;
            abort();
        }

        d->logAccessFlag = true;
    }
}

void medLogger::redirectMessage(const QString& message)
{
    dtkTrace() << message;
}

void medLogger::redirectErrorMessage(const QString& message)
{
    dtkError() << message;
}

medLogger::medLogger() : d(new medLoggerPrivate)
{
    qRegisterMetaType<QtMsgType>("QtMsgType");

    initializeTeeStreams();

    truncateLogFileIfHeavy();

    dtkLogger::instance().setLevel(logLevel);
    dtkLogger::instance().attachFile(dtkLogPath(qApp));
    dtkLogger::instance().attachConsole();

    QObject::connect(this, SIGNAL(newQtMessage(QtMsgType,QString)), this, SLOT(redirectQtMessage(QtMsgType,QString)));

    qInstallMsgHandler(qtMessageHandler);
}

medLogger::~medLogger()
{
    QObject::disconnect(this, SIGNAL(newQtMessage(QtMsgType,QString)), this, SLOT(redirectQtMessage(QtMsgType,QString)));
    qInstallMsgHandler(NULL);
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
    TeeDevice* teeDevice = new TeeDevice(*d->redirectedStreamDummies.last(), d->logFile);
    d->teeStreams.append(new TeeStream(*teeDevice));
    targetStream->rdbuf(d->teeStreams.last()->rdbuf());
}

void medLogger::truncateLogFileIfHeavy()
{
    qint64 filesize = QFileInfo(dtkLogPath(qApp)).size();

    // Over 5Mo, the file is truncated from the beginning (old lines are discarded)
    if (filesize > d->maxLogSize)
    {
        QString path = dtkLogPath(qApp);

        std::ifstream fin;
        fin.open(path.toUtf8());
        fin.seekg(filesize - d->minLogSize); // file is going to be cut to minLogSize size

        std::string keptText;
        std::string line;
        while (getline(fin,line))
        {
            keptText += line;
            keptText += "\n";
        }

        fin.close();
        std::remove(path.toUtf8());

        std::ofstream newLogFile;
        newLogFile.open(path.toUtf8());
        newLogFile << keptText;
        newLogFile.close();
    }
}

