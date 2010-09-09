#include "LoggerWidget.h"

#include <qlayout>
#include <qtextedit>
#include "LoggerMessageEvent.h"

LoggerWidget::LoggerWidget(QWidget* parent) : QWidget(parent)
{
  m_TextEdit = new QTextEdit(this);
  m_TextEdit->setTextFormat(Qt::LogText);
  m_TextEdit->setPointSize(4);
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(m_TextEdit);
}


bool LoggerWidget::event(QEvent *e)
{
    if(e->type() == LoggerMessageEvent::custom_type()) 
    {
        LoggerMessageEvent* myEvent = dynamic_cast<LoggerMessageEvent*>(e);
        m_TextEdit->append(myEvent->getMessage());
        return true;
    } 
    else 
    {
        return QWidget::event(e);
    }
}


