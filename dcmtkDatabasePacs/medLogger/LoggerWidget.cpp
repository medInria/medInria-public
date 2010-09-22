#include "LoggerWidget.h"

#include <QLayout>
#include <QTextEdit>
#include "LoggerMessageEvent.h"

LoggerWidget::LoggerWidget(QWidget* parent) : QWidget(parent)
{
  m_TextEdit = new QTextEdit(this);
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


