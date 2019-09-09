#include "medLogger.h"
#include <fstream>
#if __has_include(<filesystem>)
  #include <filesystem>
  namespace fs = std::filesystem;
#else
  #include <experimental/filesystem>
  namespace fs = std::experimental::filesystem;
#endif

class medLoggerPrivate
{
public:
    static medLogger* singleton;
    static bool logAccessFlag;

    static const qint64 maxLogSize = 5000000;
    static const qint64 minLogSize = 1000000;
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
                dtkInfo() << message;
                break;
            }
            case QtDebugMsg:
            {
                dtkDebug() << message;
                break;
            }
            case QtWarningMsg:
            {
                dtkWarn() << message;
                break;
            }
            case QtCriticalMsg:
            {
                dtkError() << message;
                break;
            }
            case QtFatalMsg:
            {
                dtkFatal() << message;
                abort();
            }
        }

        d->logAccessFlag = true;
    }
}

medLogger::medLogger() : d(new medLoggerPrivate)
{
    qRegisterMetaType<QtMsgType>("QtMsgType");

    truncateLogFileIfHeavy();

    dtkLogger::instance().setLevel(logLevel);

    dtkLogger::instance().attachFile(dtkLogPath(qApp));
    dtkLogger::instance().attachConsole();

    // Redirect cerr and cout messages
    dtkLogger::instance().redirectCerr();
    dtkLogger::instance().redirectCout();

    // Redirect Qt messages
    QObject::connect(this, SIGNAL(newQtMessage(QtMsgType,QString)), this, SLOT(redirectQtMessage(QtMsgType,QString)));
    qInstallMessageHandler(qtMessageHandler);
}

medLogger::~medLogger()
{
    dtkLogger::instance().detachFile(dtkLogPath(qApp));
    dtkLogger::instance().detachConsole();

    QObject::disconnect(this, SIGNAL(newQtMessage(QtMsgType,QString)), this, SLOT(redirectQtMessage(QtMsgType,QString)));
    qInstallMessageHandler(nullptr);
}

void medLogger::truncateLogFileIfHeavy()
{
    qint64 filesize = QFileInfo(dtkLogPath(qApp)).size();

    // Over 5Mo, the file is truncated from the beginning (old lines are discarded)
    if (filesize > d->maxLogSize)
    {
        fs::path path = dtkLogPath(qApp).toStdString();

        std::ifstream fin;
        fin.open(path);
        fin.seekg(filesize - d->minLogSize); // file is going to be cut to minLogSize size

        std::string keptText;
        std::string line;
        while (getline(fin,line))
        {
            keptText += line; //TODO redo with stringstream because it's very poor programing with string
            keptText += "\n";
        }

        fin.close();
        fs::remove(path);

        std::ofstream newLogFile;
        newLogFile.open(path);
        newLogFile << keptText;
        newLogFile.close();
    }
}

