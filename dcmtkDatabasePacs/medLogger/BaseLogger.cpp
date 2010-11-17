#include "BaseLogger.h"
#include "LoggerOutput.h"
#include "LoggerLogStream.h"

#include <sstream>
#include <iostream>
#include <ctime>


using namespace std;

BaseLogger* BaseLogger::m_Instance = 0;

//---------------------------------------------------------------------------------------------

BaseLogger::BaseLogger()
{
  //cout << "BaseLogger::BaseLogger(): BaseLogger instance created." << endl;
}

//---------------------------------------------------------------------------------------------

BaseLogger::~BaseLogger()
{
  //cout << "BaseLogger::~BaseLogger(): BaseLogger instance destroyed." << endl;
}

//---------------------------------------------------------------------------------------------

BaseLogger* BaseLogger::getInstance()
{
  if (m_Instance == 0)
  {
    m_Instance = new BaseLogger();
  } 
  return m_Instance;
}

//---------------------------------------------------------------------------------------------

void BaseLogger::destroy()
{
  if (m_Instance != 0)
  {
    delete m_Instance;
  }
}

//---------------------------------------------------------------------------------------------

void BaseLogger::startUp()
{
  // create an instance
  getInstance();
}

//---------------------------------------------------------------------------------------------

void BaseLogger::shutDown()
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

void BaseLogger::addOutput(LoggerOutput* output)
{
  BaseLogger::infoStream() << "BaseLogger::addOutput(): adding logging output " << output << " \"" << output->getName() << "\"";
  output->logMessage("LOG BEGIN", LoggerLogLevel::INFOLOG);
  getInstance()->m_Outputs.push_back(output);
}

//---------------------------------------------------------------------------------------------

void BaseLogger::removeOutput(LoggerOutput* output)
{
  BaseLogger::infoStream() << "BaseLogger::removeOutput(): removing logging output " << output << " \"" << output->getName() << "\", "
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

void BaseLogger::debug(string message)
{
  logMessage(message, LoggerLogLevel::DEBUGLOG);
}

//---------------------------------------------------------------------------------------------

void BaseLogger::info(string message)
{
  logMessage(message, LoggerLogLevel::INFOLOG);
}

//---------------------------------------------------------------------------------------------

void BaseLogger::warning(string message)
{
  logMessage(message, LoggerLogLevel::WARNINGLOG);
}

//---------------------------------------------------------------------------------------------

void BaseLogger::error(string message)
{
  logMessage(message, LoggerLogLevel::ERRORLOG);
}

//---------------------------------------------------------------------------------------------

LoggerLogStream BaseLogger::debugStream()
{
  return LoggerLogStream(LoggerLogLevel::DEBUGLOG);
}

//---------------------------------------------------------------------------------------------

LoggerLogStream BaseLogger::infoStream()
{
  return LoggerLogStream(LoggerLogLevel::INFOLOG);
}

//---------------------------------------------------------------------------------------------

LoggerLogStream BaseLogger::warningStream()
{
  return LoggerLogStream(LoggerLogLevel::WARNINGLOG);
}

//---------------------------------------------------------------------------------------------

LoggerLogStream BaseLogger::errorStream()
{
  return LoggerLogStream(LoggerLogLevel::ERRORLOG);
}

//---------------------------------------------------------------------------------------------

void BaseLogger::logMessage(string message, LoggerLogLevel::LogLevel logLevel)
{
  if (message.length() > 0)
  {
    BaseLogger* instance = getInstance();
    vector<LoggerOutput*>::iterator iter;
    for (iter = instance->m_Outputs.begin(); iter != instance->m_Outputs.end(); iter++)
    {
      (*iter)->logMessage(message, logLevel);
    }
  }
}

//---------------------------------------------------------------------------------------------

string BaseLogger::getTimeString()
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

string BaseLogger::getDateString()
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
