#include "Logger.h"
#include "LoggerOutput.h"
#include "LoggerLogStream.h"

#include <sstream>
#include <iostream>
#include <ctime>


using namespace std;

Logger* Logger::m_Instance = 0;

//---------------------------------------------------------------------------------------------

Logger::Logger()
{
  cout << "Logger::Logger(): Logger instance created." << endl;
}

//---------------------------------------------------------------------------------------------

Logger::~Logger()
{
  cout << "Logger::~Logger(): Logger instance destroyed." << endl;
}

//---------------------------------------------------------------------------------------------

Logger* Logger::getInstance()
{
  if (m_Instance == 0)
  {
    m_Instance = new Logger();
  } 
  return m_Instance;
}

//---------------------------------------------------------------------------------------------

void Logger::destroy()
{
  if (m_Instance != 0)
  {
    delete m_Instance;
  }
}

//---------------------------------------------------------------------------------------------

void Logger::startUp()
{
  // create an instance
  getInstance();
}

//---------------------------------------------------------------------------------------------

void Logger::shutDown()
{
  vector<LoggerOutput*>::iterator iter;
  while(getInstance()->m_Outputs.size() > 0)
  {
    iter = getInstance()->m_Outputs.begin();
    removeOutput(*iter);
  }
  destroy();
}

//---------------------------------------------------------------------------------------------

void Logger::addOutput(LoggerOutput* output)
{
  Logger::infoStream() << "Logger::addOutput(): adding logging output " << output << " \"" << output->getName() << "\"";
  output->logMessage("LOG BEGIN", LoggerLogLevel::INFOLOG);
  getInstance()->m_Outputs.push_back(output);
}

//---------------------------------------------------------------------------------------------

void Logger::removeOutput(LoggerOutput* output)
{
  Logger::infoStream() << "Logger::removeOutput(): removing logging output " << output << " \"" << output->getName() << "\", "
     << "(currently " << getInstance()->m_Outputs.size() << " outputs).";
  vector<LoggerOutput*>::iterator iter;
  vector<LoggerOutput*>::iterator removeIter = getInstance()->m_Outputs.end();
  for (iter = getInstance()->m_Outputs.begin(); iter != getInstance()->m_Outputs.end(); iter++)
  {
    if ((*iter) == output)
    {
      removeIter = iter; // store position to delete
    }
  }
  if (removeIter != getInstance()->m_Outputs.end())
  {
    (*removeIter)->logMessage("LOG END", LoggerLogLevel::INFOLOG);
    getInstance()->m_Outputs.erase(removeIter);
  }
}

//---------------------------------------------------------------------------------------------

void Logger::debug(string message)
{
  logMessage(message, LoggerLogLevel::DEBUGLOG);
}

//---------------------------------------------------------------------------------------------

void Logger::info(string message)
{
  logMessage(message, LoggerLogLevel::INFOLOG);
}

//---------------------------------------------------------------------------------------------

void Logger::warning(string message)
{
  logMessage(message, LoggerLogLevel::WARNINGLOG);
}

//---------------------------------------------------------------------------------------------

void Logger::error(string message)
{
  logMessage(message, LoggerLogLevel::ERRORLOG);
}

//---------------------------------------------------------------------------------------------

LoggerLogStream Logger::debugStream()
{
  return LoggerLogStream(LoggerLogLevel::DEBUGLOG);
}

//---------------------------------------------------------------------------------------------

LoggerLogStream Logger::infoStream()
{
  return LoggerLogStream(LoggerLogLevel::INFOLOG);
}

//---------------------------------------------------------------------------------------------

LoggerLogStream Logger::warningStream()
{
  return LoggerLogStream(LoggerLogLevel::WARNINGLOG);
}

//---------------------------------------------------------------------------------------------

LoggerLogStream Logger::errorStream()
{
  return LoggerLogStream(LoggerLogLevel::ERRORLOG);
}

//---------------------------------------------------------------------------------------------

void Logger::logMessage(string message, LoggerLogLevel::LogLevel logLevel)
{
  if (message.length() > 0)
  {
    Logger* instance = getInstance();
    vector<LoggerOutput*>::iterator iter;
    for (iter = instance->m_Outputs.begin(); iter != instance->m_Outputs.end(); iter++)
    {
      (*iter)->logMessage(message, logLevel);
    }
  }
}

//---------------------------------------------------------------------------------------------

string Logger::getTimeString()
{
  time_t currentTime;
  time (&currentTime); // fill now with the current time

  struct tm* ptm = localtime(&currentTime);

  ostringstream os;
  os.setf(ios::right, ios::adjustfield);
  os.width(2);
  os.fill('0');
  os << ptm->tm_hour << ":";
  os.width(2);
  os.fill('0');
  os << ptm->tm_min << ":";
  os.width(2);
  os.fill('0');
  os << ptm->tm_sec;
  return os.str();
}

//---------------------------------------------------------------------------------------------

string Logger::getDateString()
{
  time_t currentTime;
  time (&currentTime); // fill now with the current time

  struct tm* ptm = localtime(&currentTime);

  ostringstream os;
  os.setf(ios::right, ios::adjustfield);
  os.width(2);
  os.fill('0');
  os << ptm->tm_mday << ".";
  os.width(2);
  os.fill('0');
  os << ptm->tm_mon + 1 << ".";
  os.width(2);
  os.fill('0');
  os << ptm->tm_year - 100 << ": ";
  return os.str();
}

//---------------------------------------------------------------------------------------------
