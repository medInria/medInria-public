/* medMessageController.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 09:59:08 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 12:41:07 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 143
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <dtkCore/dtkGlobal.h>

#include "medMessageController.h"

#include <dtkCore/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessage
// /////////////////////////////////////////////////////////////////

medMessageControllerMessage::medMessageControllerMessage(QObject* sender,
                                                         QWidget *parent) : QWidget(parent)
{
    this->sender = sender;
    widget = new QWidget(this);
    widget->setFixedWidth(400);
}

medMessageControllerMessage::~medMessageControllerMessage(void)
{

}

QWidget * medMessageControllerMessage::getWidget()
{
  return widget;
}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageSimple
// /////////////////////////////////////////////////////////////////

medMessageControllerMessageSimple::medMessageControllerMessageSimple(
        QObject* sender, const QString& text,
        QWidget *parent,unsigned int timeout) : medMessageControllerMessage(sender,parent)
{    
    icon = new QLabel(this);
    //icon->setPixmap(QPixmap(":/icons/information.png"));

    QLabel *info = new QLabel(this);
    info->setText(text);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignLeft);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(icon);
    layout->addWidget(info);

    if ( timeout > 0 ){
        this->timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(remove()));
        timer->start(timeout);
    }

    widget->setLayout(layout);
}
void medMessageControllerMessageSimple::remove(){
    medMessageController::instance()->remove(sender);
}


medMessageControllerMessageSimple::~medMessageControllerMessageSimple(void)
{

}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageInfo
// /////////////////////////////////////////////////////////////////

medMessageControllerMessageInfo::medMessageControllerMessageInfo(
        QObject* sender, const QString& text, QWidget *parent,unsigned int timeout) :
        medMessageControllerMessageSimple(sender,text, parent,timeout)
{
    icon->setPixmap(QPixmap(":/icons/information.png"));
}

medMessageControllerMessageInfo::~medMessageControllerMessageInfo(void)
{

}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageError
// /////////////////////////////////////////////////////////////////

medMessageControllerMessageError::medMessageControllerMessageError(
        QObject* sender, const QString& text, QWidget *parent,unsigned int timeout) :
        medMessageControllerMessageSimple(sender, text, parent,timeout)
{
    icon->setPixmap(QPixmap(":/icons/exclamation.png"));
}

medMessageControllerMessageError::~medMessageControllerMessageError(void)
{

}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageProgress
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageProgressPrivate
{
public:
    QProgressBar *progress;
};

medMessageControllerMessageProgress::medMessageControllerMessageProgress(
        QObject* sender, const QString& text, QWidget *parent) :
        medMessageControllerMessage(sender,parent), d(new medMessageControllerMessageProgressPrivate)
{ 
    QLabel *icon = new QLabel(this);
    icon->setPixmap(QPixmap(":/icons/information.png"));

    QLabel *info = new QLabel(this);
    info->setText(text);
    
    d->progress = new QProgressBar(this);
    d->progress->setMinimum(0);
    d->progress->setMaximum(100);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignLeft);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(icon);
    layout->addWidget(info);
    layout->addWidget(d->progress);

    widget->setLayout(layout);
}

medMessageControllerMessageProgress::~medMessageControllerMessageProgress(void)
{
    delete d;
    d = NULL;
}

void medMessageControllerMessageProgress::setProgress(int value)
{
    d->progress->setValue(value);
}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageQuestion
// /////////////////////////////////////////////////////////////////

class medMessageControllerMessageQuestionPrivate
{
public:
};

medMessageControllerMessageQuestion::medMessageControllerMessageQuestion(QObject* sender, const QString& text, QWidget *parent) : medMessageControllerMessage(sender,parent), d(new medMessageControllerMessageQuestionPrivate)
{  
    QLabel *icon = new QLabel(this);
    icon->setPixmap(QPixmap(":/icons/information.png"));

    QLabel *info = new QLabel(this);
    info->setText(text);
    
    QPushButton *ok_button = new QPushButton("Yes", this);
    QPushButton *no_button = new QPushButton("No", this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignLeft);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(icon);
    layout->addWidget(info);
    layout->addWidget(ok_button);
    layout->addWidget(no_button);

    connect(ok_button, SIGNAL(clicked()), this, SIGNAL(accepted()));
    connect(no_button, SIGNAL(clicked()), this, SIGNAL(rejected()));

    widget->setLayout(layout);
}

medMessageControllerMessageQuestion::~medMessageControllerMessageQuestion(void)
{
    delete d;
}

// /////////////////////////////////////////////////////////////////
// medMessageController
// /////////////////////////////////////////////////////////////////

class medMessageControllerPrivate
{
public:
    QHash<QObject *, medMessageControllerMessage *> messages;
};

medMessageController *medMessageController::instance(void)
{
    if(!s_instance)
        s_instance = new medMessageController;

    return s_instance;
}

void medMessageController::showInfo(QObject *sender, const QString& text,unsigned int timeout)
{
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) {
        // GUI
        medMessageControllerMessageInfo *message = new medMessageControllerMessageInfo(
                sender,text,0,timeout);

        emit addMessage(message->getWidget());
        
        d->messages.insert(sender, message);
    } else {
        dtkOutput() << text;
    }
}

void medMessageController::showError(QObject *sender, const QString& text,unsigned int timeout)
{
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) {
        // GUI
        medMessageControllerMessageError *message = new medMessageControllerMessageError(
                sender,text,0,timeout);

        emit addMessage(message->getWidget());
        
        d->messages.insert(sender, message);
    } else {
        dtkError() << text;
    }
}

void medMessageController::showProgress(QObject *sender, const QString& text)
{
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) {
        // GUI
        medMessageControllerMessageProgress *message = new medMessageControllerMessageProgress(sender,text);

        emit addMessage(message->getWidget());
        
        d->messages.insert(sender, message);
    } else {
        dtkDebug() << "Progress : " << text;
    }

}

void medMessageController::setProgress(int value)
{
    medMessageControllerMessage *message = d->messages.value(this->sender());

    if(medMessageControllerMessageProgress *progress = dynamic_cast<medMessageControllerMessageProgress *>(message)) {
        progress->setProgress(value);
        progress->update();
        qApp->processEvents();
    }
}

void medMessageController::remove(QObject *sender)
{
    medMessageControllerMessage *message = d->messages.value(sender);

    emit removeMessage(message->getWidget());

    d->messages.remove(sender);

    delete message;
}

medMessageController::medMessageController(void) : QObject(), d(new medMessageControllerPrivate)
{
}

medMessageController::~medMessageController(void)
{
    delete d;

    d = NULL;
}

medMessageController *medMessageController::s_instance = NULL;
