#include "LoggerWidgetOutput.h"

#include <QApplication>

#include "LoggerMessageEvent.h"

using namespace std;

//---------------------------------------------------------------------------------------------

LoggerWidgetOutput::LoggerWidgetOutput(QWidget* parent, LoggerLogLevel::LogLevel logLevel) : 
    LoggerOutput(logLevel, "WidgetOutput")
{
    m_parent = parent;
}

//---------------------------------------------------------------------------------------------

LoggerWidgetOutput::~LoggerWidgetOutput()
{
}

//---------------------------------------------------------------------------------------------

void LoggerWidgetOutput::logMessage(std::string message, LoggerLogLevel::LogLevel logLevel)
{
  QString msg;
  if (logLevel <= this->getLogLevel())
  {
    msg.append("[");
    msg.append(Logger::getTimeString().c_str());
    msg.append("] ");
    msg.append("<font color=");
    switch(logLevel)
    {
      case LoggerLogLevel::INFOLOG:
        msg.append("\"#000000\"><b>(INF)</b> ");
        break;
      case LoggerLogLevel::WARNINGLOG:
        msg.append("\"#FF9900\"><b>(WAR)</b> ");
        break;
      case LoggerLogLevel::ERRORLOG:
        msg.append("\"#FF0000\"><b>(ERR)</b> ");
        break;
      default:
        break;
    }
    msg.append(message.c_str());
    msg.append("</font>");
    // sending message to gui
    QApplication::postEvent(m_parent, new LoggerMessageEvent(msg));
  }
}

//---------------------------------------------------------------------------------------------

