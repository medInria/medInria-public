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

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessage
// /////////////////////////////////////////////////////////////////

medMessageControllerMessage::medMessageControllerMessage(QObject* sender,
                                                         QWidget *parent) : QWidget(parent)
{
    this->setFixedWidth(400);
    this->sender = sender;
}

medMessageControllerMessage::~medMessageControllerMessage(void)
{

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
    QStatusBar *status;

    QHash<QObject *, medMessageControllerMessage *> messages;
};

medMessageController *medMessageController::instance(void)
{
    if(!s_instance)
        s_instance = new medMessageController;

    return s_instance;
}

void medMessageController::attach(QStatusBar *status)
{
    d->status = status;
}

void medMessageController::showInfo(QObject *sender, const QString& text,unsigned int timeout)
{
    medMessageControllerMessageInfo *message = new medMessageControllerMessageInfo(
            sender,text,0,timeout);
    
    d->status->addWidget(message);
    d->status->update();
    qApp->processEvents();

    d->messages.insert(sender, message);
}

void medMessageController::showError(QObject *sender, const QString& text,unsigned int timeout)
{
    medMessageControllerMessageError *message = new medMessageControllerMessageError(
            sender,text,0,timeout);

    d->status->addWidget(message);
    d->status->update();
    qApp->processEvents();
    d->messages.insert(sender, message);
}

void medMessageController::showProgress(QObject *sender, const QString& text)
{
    medMessageControllerMessageProgress *message = new medMessageControllerMessageProgress(sender,text);
    
    d->status->addWidget(message);
    d->status->update();
    qApp->processEvents();

    d->messages.insert(sender, message);
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

    d->status->removeWidget(message);

    d->messages.remove(sender);

    delete message;
}

medMessageController::medMessageController(void) : QObject(), d(new medMessageControllerPrivate)
{
    d->status = NULL;
}

medMessageController::~medMessageController(void)
{
    delete d;

    d = NULL;
}

medMessageController *medMessageController::s_instance = NULL;
