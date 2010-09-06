#include <iostream>

#include "LoggerLogStream.h"
#include "Logger.h"

using namespace std;

LoggerLogStream::LoggerLogStream(LoggerLogLevel::LogLevel logLevel)
{
  m_LogLevel = logLevel;
  m_Buffer = new ostringstream();
}

LoggerLogStream::~LoggerLogStream()
{
  flushBuffer();
  delete m_Buffer;
}

void LoggerLogStream::flushBuffer()
{
  string bufferString = m_Buffer->str();
  m_Buffer->str("");
  if (bufferString.length() > 0)
  {
    Logger::logMessage(bufferString, m_LogLevel);
  }
}
