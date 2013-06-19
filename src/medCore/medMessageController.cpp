/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCore/dtkGlobal.h>

#include "medMessageController.h"

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medMessage
// /////////////////////////////////////////////////////////////////

medMessage::medMessage( QWidget *parent,
                        const QString& text, 
                        unsigned int timeout) : QWidget(parent)
{
    this->timeout = timeout;

    this->setFixedWidth(400);
    icon = new QLabel(this);

    info = new QLabel(this);
    info->setText(text);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignLeft);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(icon);
    layout->addWidget(info);

    if ( timeout > 0 )
    {
        this->timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(remove()));
    }

    this->setLayout(layout);
}

medMessage::~medMessage(void)
{

}

void medMessage::startTimer()
{
    if (timeout>0)
        timer->start(timeout);
}

void medMessage::stopTimer()
{
    if (timeout>0)
        timer->stop();
}

void medMessage::remove()
{
    medMessageController::instance()->remove(this);
}


// /////////////////////////////////////////////////////////////////
// medMessageInfo
// /////////////////////////////////////////////////////////////////

medMessageInfo::medMessageInfo(
        const QString& text, QWidget *parent,unsigned int timeout) :
        medMessage(parent,text, timeout)
{
    icon->setPixmap(QPixmap(":/icons/information.png"));
    this->setFixedWidth(200);
}

medMessageInfo::~medMessageInfo(void)
{

}

// /////////////////////////////////////////////////////////////////
// medMessageError
// /////////////////////////////////////////////////////////////////

medMessageError::medMessageError(
        const QString& text, QWidget *parent,unsigned int timeout) :
        medMessage(parent, text, timeout)
{
    icon->setPixmap(QPixmap(":/icons/exclamation.png"));
    this->setFixedWidth(350);
}

medMessageError::~medMessageError(void)
{

}

// /////////////////////////////////////////////////////////////////
// medMessageProgress
// /////////////////////////////////////////////////////////////////


medMessageProgress::medMessageProgress(
        const QString& text, QWidget *parent) :
        medMessage(parent, text, 0)
{
    progress = new QProgressBar(this);
    progress->setMinimum(0);
    progress->setMaximum(100);
    progress->setValue(100);
    this->layout()->addWidget(progress);
}

medMessageProgress::~medMessageProgress(void)
{
}

void medMessageProgress::setProgress(int value)
{
    progress->setValue(value);
}

void medMessageProgress::success(void)
{
    progress->setStyleSheet("QProgressBar::chunk {background-color: lime;}");
    this->associateTimer();
    info->setText("Operation succeeded");

}

void medMessageProgress::failure(void)
{
    progress->setStyleSheet("QProgressBar::chunk {background-color: red;}");
    this->associateTimer();
    info->setText("Operation aborted");
}
void medMessageProgress::associateTimer(void)
{
    this->timer = new QTimer(this);
    int timeout = 2000;
    connect(timer, SIGNAL(timeout()), this, SLOT(remove()));
    timer->start(timeout);
}

// /////////////////////////////////////////////////////////////////
// medMessageController
// /////////////////////////////////////////////////////////////////


medMessageController *medMessageController::instance(void)
{
    if(!s_instance)
        s_instance = new medMessageController;

    return s_instance;
}

void medMessageController::showInfo(const QString& text,unsigned int timeout)
{
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) 
    {
        // GUI
        medMessageInfo *message = new medMessageInfo(
                text,0,timeout);
        emit addMessage(message);
    } else {
        dtkTrace() << text;
    }
}

void medMessageController::showError(const QString& text,unsigned int timeout)
{
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) 
    {
        // GUI
        medMessageError *message = new medMessageError(
                text,0,timeout);
        emit addMessage(message);

    } else {
        dtkError() << text;
    }
}

medMessageProgress * medMessageController::showProgress(const QString& text)
{
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) 
    {
        // GUI
        medMessageProgress *message = new medMessageProgress(text);

        emit addMessage(message);
        return message;

    } 
}


void medMessageController::remove(medMessage *message)
{
    if(message != NULL)
    {
        emit removeMessage(message);
        message->deleteLater();
    }
}

medMessageController::medMessageController(void) : QObject()
{
}

medMessageController::~medMessageController(void)
{
}

medMessageController *medMessageController::s_instance = NULL;
