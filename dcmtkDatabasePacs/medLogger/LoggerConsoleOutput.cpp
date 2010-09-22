#include "LoggerConsoleOutput.h"

#include "BaseLogger.h"
#include <iostream>

using namespace std;

//---------------------------------------------------------------------------------------------

LoggerConsoleOutput::LoggerConsoleOutput(LoggerLogLevel::LogLevel logLevel) : LoggerOutput(logLevel, "ConsoleOutput")
{
  ostringstream msg;
  msg << "LoggerConsoleOutput::LoggerConsoleOutput(): New logger output to console with log level " << getLogLevel() << " created.";
  BaseLogger::info(msg.str());
}

//---------------------------------------------------------------------------------------------

LoggerConsoleOutput::~LoggerConsoleOutput()
{
}

//---------------------------------------------------------------------------------------------

void LoggerConsoleOutput::logMessage(std::string message, LoggerLogLevel::LogLevel logLevel)
{
  if (logLevel <= this->getLogLevel())
  {
    cout << "[" << BaseLogger::getTimeString() << "] ";
    switch(logLevel)
    {
      case LoggerLogLevel::DEBUGLOG:
        cout << "DEB: ";
        break;
      case LoggerLogLevel::INFOLOG:
        cout << "INF: ";
        break;
      case LoggerLogLevel::WARNINGLOG:
        cout << "WAR: ";
        break;
      case LoggerLogLevel::ERRORLOG:
        cout << "ERR: ";
        break;
      default:
        break;
    }
    cout << message << endl;
  }
}

//---------------------------------------------------------------------------------------------

