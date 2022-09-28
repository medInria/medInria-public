/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medNewLogger.h"

#include <medLog.h>
#include <medLogStream.h>


#include <QtCore>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>

#include <iostream>
#include <fstream>
#include <string>

#include <teeStream.tpp>
#include <medNotifSys.h>

#if defined(WIN32)
#include <windows.h>

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


class captureStream : public std::basic_streambuf<char>
{
public:
    captureStream(std::ostream& stream, medNewLogger &logger) : m_stream(stream), m_logger(logger)
    {
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }

    ~captureStream()
    {
        m_stream.rdbuf(m_old_buf);
    }

protected:
    //This is called when a std::endl has been inserted into the stream
    virtual int_type overflow(int_type v)
    {
        if (v == '\n' || v == '\r')
        {
            m_logger.writeMsgfromStdStream(&m_stream, QString(v));
        }

        return v;
    }

    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_logger.writeMsgfromStdStream(&m_stream, QString(QByteArray(p, n)));
        return n;
    }

private:
    medNewLogger   & m_logger;
    std::ostream   & m_stream;
    std::streambuf * m_old_buf;
};

class medNewLoggerPrivate
{
public:
    static medNewLogger* singleton;
    static medNewLogger* qtMsgHandlerInstance;

    QString path;
    QApplication * app;
    
    QString mainLogFileName;
    std::ofstream * mainLogFile;
    
    QMap<QString, std::ofstream *> logFilesMap;
    QMap<QString, sLogLevel>  logFilesPropertyMap;

    QMap<medLog *, streamProperty>      medStreamCombinationMap;
    QMap<std::ostream*, streamProperty> stdStreamCombinationMap;
    QMap<QtMsgType, streamProperty>     qtmsgtypeCombinationMap;

    QMap<std::ostream*, captureStream*> stdStreamBuffMap;

    static qint64 maxLogSize;
    static qint64 minLogSize;
};

medNewLogger* medNewLoggerPrivate::singleton = nullptr;
medNewLogger* medNewLoggerPrivate::qtMsgHandlerInstance = nullptr;
qint64 medNewLoggerPrivate::maxLogSize = 5 * 1024 * 1024;
qint64 medNewLoggerPrivate::minLogSize = 1 * 1024 * 1024;

medLog medNewLogger::medLogWarning(medWarningMsg);
medLog medNewLogger::medLogDebug(medDebugMsg);
medLog medNewLogger::medLogCritical(medErrorMsg);
medLog medNewLogger::medLogFatal(medFatalMsg);
medLog medNewLogger::medLogInfo(medInfoMsg);





// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// medNewLogger
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medNewLogger::medNewLogger() : d(new medNewLoggerPrivate)
{
    qRegisterMetaType<QtMsgType>("QtMsgType");

    d->qtmsgtypeCombinationMap[QtDebugMsg];
    d->qtmsgtypeCombinationMap[QtWarningMsg];
    d->qtmsgtypeCombinationMap[QtCriticalMsg];
    d->qtmsgtypeCombinationMap[QtFatalMsg];
    d->qtmsgtypeCombinationMap[QtInfoMsg];

    d->logFilesPropertyMap["cout"].levelArray;
    d->logFilesPropertyMap["cerr"].levelArray;
    d->logFilesPropertyMap["dbgConsole"].levelArray;
    
    d->mainLogFile = nullptr;

    for (auto fileName : d->logFilesMap.keys())
    {
        truncateLogFileIfHeavy(filePath(fileName));
    }
}

medNewLogger::~medNewLogger()
{    
    if (medNewLoggerPrivate::singleton == this)
    {
        qInstallMessageHandler(nullptr);
    }
}

medNewLogger& medNewLogger::mainInstance()
{
    if (medNewLoggerPrivate::singleton == nullptr)
    {
        medNewLoggerPrivate::singleton = new medNewLogger();
    }

    return *medNewLoggerPrivate::singleton;
}

bool medNewLogger::initialize(medNewLogger * instance)
{
    bool bRes = false;

    instance->enableQtRedirection(true);

    instance->registerStdStream(std::cout);
    instance->registerStdStream(std::cerr);

    instance->registerMedStream(medNewLogger::medLogWarning);
    instance->registerMedStream(medNewLogger::medLogDebug);
    instance->registerMedStream(medNewLogger::medLogCritical);
    instance->registerMedStream(medNewLogger::medLogFatal);
    instance->registerMedStream(medNewLogger::medLogInfo);
     //TODO
    return bRes;
}

void medNewLogger::finalize()
{
    //TODO
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



void medNewLogger::qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    medNewLoggerPrivate::qtMsgHandlerInstance->redirectQtMessage(type, context, QString(message));
}

void medNewLogger::redirectQtMessage(QtMsgType type, const QMessageLogContext &context, const QString& message)
{
    QString sContext;
    QString sMsg = message;

    bool bNoContext = context.file == nullptr && context.function == nullptr && context.line > 0;    
    bNoContext ? sContext="" : sContext = QString(context.file) + QString(':') + QString(context.function) + QString(':') + QString::number(context.line);    

    auto level = getLevelOfQt(type); 
    auto prop = d->qtmsgtypeCombinationMap[type];
    if (prop.context && !bNoContext)
    {
        sMsg = sContext + message;
    }
    writeMsgInternal(level, prop, sMsg);
}

QString medNewLogger::filePath(QString const & fileName)
{
    QString pathRes = d->path.isEmpty() ? "./" + fileName : d->path + "/" + fileName;

    return QDir::toNativeSeparators(pathRes);
}


bool medNewLogger::outputIsEnable(QString outputName, eLogLevel level)
{
    bool bRes = false;

    if (d->logFilesPropertyMap.contains(outputName))
    {
        bRes = d->logFilesPropertyMap[outputName][level];
    }

    return bRes;
}

eLogLevel medNewLogger::getLevelOfMsg(QString const & msg)
{
    eLogLevel eLevelRes = medInfoMsg;
    
    if (msg.startsWith("[DBG]")) {eLevelRes = medDebugMsg;}
    else if (msg.startsWith("[INF]")) {eLevelRes = medInfoMsg;}
    else if (msg.startsWith("[WRN]")) {eLevelRes = medWarningMsg;}
    else if (msg.startsWith("[CRT]")) {eLevelRes = medErrorMsg;}
    else if (msg.startsWith("[FAT]")) {eLevelRes = medDebugMsg;}
    
    return eLevelRes;
}

/** Return the log level associated to a QtMsgType.
 * @brief this helper function return the log level associated to a QtMsgType.
 * @param type the QtMsgType to map on eLogLevel.
 * @return the eLogLevel associated to a QtMsgType.
 * @details this function is call when Qt redirection appears throw redirectQtMessage.
 */
eLogLevel medNewLogger::getLevelOfQt(QtMsgType const type)
{
    eLogLevel levelRes = eLogLevel::medInfoMsg;

    switch (type)
    {
        case QtMsgType::QtDebugMsg:    levelRes = eLogLevel::medDebugMsg;   break;
        case QtMsgType::QtInfoMsg:     levelRes = eLogLevel::medInfoMsg;    break;
        case QtMsgType::QtWarningMsg:  levelRes = eLogLevel::medWarningMsg; break;
        case QtMsgType::QtCriticalMsg: levelRes = eLogLevel::medErrorMsg;   break;
        case QtMsgType::QtFatalMsg:    levelRes = eLogLevel::medFatalMsg;   break;
    }

    return levelRes;
}


/** Test the size of the log file and cut if needed
 * @brief this helper function truncate a log file if this one it's become too heavy
 * @param path to the file
 * @details this function is call when the log file is open (creation or append)
 */
void medNewLogger::truncateLogFileIfHeavy(QString const & path)
{
    qint64 filesize = QFileInfo(path).size();
    
    // Over 5Mo, the file is truncated from the beginning (old lines are discarded)
    if (filesize > medNewLoggerPrivate::maxLogSize)
    {
    
        QFile inFile(path);
        inFile.open(QFile::ReadOnly);
        inFile.seek(filesize - medNewLoggerPrivate::minLogSize); // file is going to be cut to minLogSize size
    
        QTextStream keptText;
        while (!inFile.atEnd())
        {
            keptText << inFile.readLine();
            keptText << "\n";
        }
    
        inFile.remove();
        inFile.close();
    
        QFile outFile(path);
        outFile.open(QFile::ReadWrite | QFile::Truncate);
        outFile.write(keptText.string()->toUtf8());
    
        outFile.close();
    }
}



bool medNewLogger::setLogPath(QString path)
{
    bool bRes = QFileInfo(path).isDir();

    if (bRes)
    {
        d->path = path;
    }

    return bRes;
}

void medNewLogger::setLogApp(QApplication * app)
{
    d->app = app;
}

bool medNewLogger::setLogOutputFileProperty(QString fileName, sLogLevel property)
{
    bool bRes = true;

    if (d->logFilesPropertyMap.contains(fileName))
    {
        d->logFilesPropertyMap[fileName] = property;
    }
    else
    {
        createOutputFileIfNeeded(fileName);
    }

    return bRes;
}

bool medNewLogger::enableQtRedirection(bool enable)
{
    bool bRes = medNewLoggerPrivate::qtMsgHandlerInstance == nullptr || medNewLoggerPrivate::qtMsgHandlerInstance == this;
    
    if (bRes)
    {
        if (enable)
        {
            medNewLoggerPrivate::qtMsgHandlerInstance = this;
            qInstallMessageHandler(qtMessageHandler);
        }
        else
        {
            medNewLoggerPrivate::qtMsgHandlerInstance = nullptr;
            qInstallMessageHandler(nullptr);
        }
    }

    return bRes;
}






void medNewLogger::registerMedStream(medLog & log, streamProperty prop)
{
    d->medStreamCombinationMap[&log] = prop;
    createOutputFileIfNeeded(prop.fileName);
}

void medNewLogger::registerStdStream(std::ostream & log, streamProperty prop)
{
    d->stdStreamCombinationMap[&log] = prop;
    d->stdStreamBuffMap[&log] = new captureStream(log, *this);
    createOutputFileIfNeeded(prop.fileName);
}

void medNewLogger::createOutputFileIfNeeded(QString fileName)
{
    if (!fileName.isEmpty() && !d->logFilesMap.contains(fileName))
    {
        d->logFilesMap[fileName] = new std::ofstream();
        d->logFilesMap[fileName]->open(convertUTF8toLocalUtfString(filePath(fileName).toStdString().c_str()), std::ios::app);
        d->logFilesPropertyMap[fileName];
        truncateLogFileIfHeavy(filePath(fileName));
    }
}

void medNewLogger::writeMsgfromMedStream(medLog * log, QString &data, streamProperty *property)
{
    //TODO

    if (d->medStreamCombinationMap.contains(log))
    {
        auto & prop = d->medStreamCombinationMap[log];
        writeMsgInternal(getLevelOfMsg(data), prop, data);
    }
    else
    {
        //TODO
    }
}

void medNewLogger::writeMsgfromStdStream(std::ostream * log, QString const & data)
{
    if (d->stdStreamCombinationMap.contains(log))
    {
        auto & prop = d->stdStreamCombinationMap[log];
        writeMsgInternal(getLevelOfMsg(data), prop, data);
    }
    else
    {
        fprintf(stderr, "ostream with pointer %p is not registred in medLogger. It contains the following message: %s\n", log, data.toStdString().c_str() );
    }
}


void medNewLogger::writeMsgInternal(eLogLevel level, streamProperty const & prop, QString const & message)
{
    auto msg = message.toUtf8().toStdString();
    if (prop.file)
    {
        std::ostream * outputStream = nullptr;
        QString fileName;

        if (prop.fileName.isEmpty())
        {
            fileName = d->mainLogFileName;
            outputStream = d->mainLogFile;
        }
        else
        {
            if (d->logFilesMap.contains(prop.fileName))
            {
                fileName = prop.fileName;
                outputStream = d->logFilesMap[fileName];
            }
        }
        if (outputIsEnable(fileName, level) || (level == eLogLevel::medNoneMsg && fileName == d->mainLogFileName))
        {
           (*outputStream) << msg << std::endl;
        }
        
    }
    if (prop.coutConsole && (outputIsEnable("cout", level) || level == eLogLevel::medNoneMsg))
    {
        fprintf(stdout, "%s\n", msg.c_str());
    }
    if (prop.cerrConsole && outputIsEnable("cerr", level))
    {
        fprintf(stderr, "%s\n", msg.c_str());
    }
#ifdef _MSC_VER 
    if (prop.dbgConsole && outputIsEnable("dbgConsole", level))
    {
        auto str = convertUTF8toLocalUtfString(msg.c_str());
        OutputDebugStringW(str.c_str());
    }
#endif // _MSC_VER
}
