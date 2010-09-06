#include <sstream>

#include "LoggerOutput.h"

LoggerOutput::LoggerOutput(LoggerLogLevel::LogLevel logLevel, std::string name)
{
  m_LogLevel = logLevel;
  m_Name = name;
}

LoggerOutput::~LoggerOutput()
{
}

void LoggerOutput::setLogLevel(LoggerLogLevel::LogLevel level)
{
  m_LogLevel = LoggerLogLevel::INFOLOG;
  std::ostringstream msg;
  msg << "LoggerOutput::setLogLevel(): switching log level of this output to " << level << ".";
  logMessage(msg.str(), LoggerLogLevel::INFOLOG);
  m_LogLevel = level;
}

LoggerLogLevel::LogLevel LoggerOutput::getLogLevel() const
{
  return m_LogLevel;
}

std::string LoggerOutput::getName() const
{
  return m_Name;
}
