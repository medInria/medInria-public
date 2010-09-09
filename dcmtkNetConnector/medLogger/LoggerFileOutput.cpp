#include "LoggerFileOutput.h"

#include <sstream>

using namespace std;

//---------------------------------------------------------------------------------------------

LoggerFileOutput::LoggerFileOutput(LoggerLogLevel::LogLevel logLevel, const char* filename) : 
    LoggerOutput(logLevel, "FileOutput"), m_OutputStream(filename)
{
  m_SequenceNumber = 0;
  ostringstream msg;
  msg << "LoggerFileOutput::LoggerFileOutput(): New file logger output to file '" 
        << filename << "' with log level " << getLogLevel() << " created.";
  Logger::info(msg.str());
}

//---------------------------------------------------------------------------------------------

LoggerFileOutput::~LoggerFileOutput()
{
  m_OutputStream.close();
}

//---------------------------------------------------------------------------------------------

void LoggerFileOutput::logMessage(std::string message, LoggerLogLevel::LogLevel logLevel)
{
  if (logLevel <= this->getLogLevel())
  {
    // write sequence number
    m_OutputStream << "[";
    m_OutputStream.fill('0');
    m_OutputStream.width(8);
    m_OutputStream << m_SequenceNumber++ << "] ";
    // write time
    m_OutputStream << "[" << Logger::getTimeString() << "] ";

    switch(logLevel)
    {
      case LoggerLogLevel::DEBUGLOG:
        m_OutputStream << "DEB: ";
        break;
      case LoggerLogLevel::INFOLOG:
        m_OutputStream << "INF: ";
        break;
      case LoggerLogLevel::WARNINGLOG:
        m_OutputStream << "WAR: ";
        break;
      case LoggerLogLevel::ERRORLOG:
        m_OutputStream << "ERR: ";
        break;
      default:
        break;
    }
    m_OutputStream << message << endl;
  }
}

//---------------------------------------------------------------------------------------------

