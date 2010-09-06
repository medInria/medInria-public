#include "LoggerWidgetOutput.h"

#include <qlayout.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qregexp.h>
#include <qmessagebox.h>

using namespace std;

LoggerWidgetOutput::LoggerWidgetOutput(LoggerLogLevel::LogLevel logLevel, QWidget* parent) : QWidget(parent), LoggerOutput(logLevel, "WidgetOutput")
{
  m_TextEdit = new QTextEdit(this);
  m_TextEdit->setTextFormat(Qt::LogText);
  m_TextEdit->setPointSize(4);
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(m_TextEdit);

  //QPushButton* button = new QPushButton("Copy to clipboard", this);
  //layout->addWidget(button);

  //connect(button, SIGNAL(clicked()), this, SLOT(copyToClipboard()));
}

LoggerWidgetOutput::~LoggerWidgetOutput()
{
}

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
    m_TextEdit->append(msg);
  }
  if (logLevel == LoggerLogLevel::ERRORLOG)
  {
    QMessageBox::critical(0, "An error occured", msg);
  }
}

void LoggerWidgetOutput::copyToClipboard()
{
  QClipboard *cb = QApplication::clipboard();
  QString text = m_TextEdit->text();
  // remove tags
  text.replace(QRegExp("<[^>]*>"), "");
  cb->setText(text, QClipboard::Clipboard);
}
