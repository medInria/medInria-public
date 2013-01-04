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

#include <dtkLog/dtkLog.h>

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessage
// /////////////////////////////////////////////////////////////////

medMessageControllerMessage::medMessageControllerMessage(QObject* sender,
                                                         QWidget *parent,
														 const QString& text, 
														 unsigned int timeout, 
														 bool messageProgress) : QWidget(parent)
{
    this->sender = sender;
    widget = new QWidget(this);
    widget->setFixedWidth(400);
	
	icon = new QLabel(this);

    info = new QLabel(this);
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

	if (messageProgress){
		progress = new QProgressBar(this);
		progress->setMinimum(0);
		progress->setMaximum(100);
		layout->addWidget(progress);
	}

    widget->setLayout(layout);
}

medMessageControllerMessage::~medMessageControllerMessage(void)
{

}

QWidget * medMessageControllerMessage::getWidget(){
	return widget;
}

void medMessageControllerMessage::remove(){
	medMessageController::instance()->remove(this);
}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageInfo
// /////////////////////////////////////////////////////////////////

medMessageControllerMessageInfo::medMessageControllerMessageInfo(
        const QString& text, QWidget *parent,unsigned int timeout) :
        medMessageControllerMessage(sender,parent,text, timeout)
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
        const QString& text, QWidget *parent,unsigned int timeout) :
        medMessageControllerMessage(sender,parent, text, timeout)
{
    icon->setPixmap(QPixmap(":/icons/exclamation.png"));
}

medMessageControllerMessageError::~medMessageControllerMessageError(void)
{

}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageProgress
// /////////////////////////////////////////////////////////////////


medMessageControllerMessageProgress::medMessageControllerMessageProgress(
        QObject* sender, const QString& text, QWidget *parent) :
        medMessageControllerMessage(sender,parent, text, 0, true)
{
}

medMessageControllerMessageProgress::~medMessageControllerMessageProgress(void)
{
}

void medMessageControllerMessageProgress::setProgress(int value)
{
    progress->setValue(value);
}

void medMessageControllerMessageProgress::successMessage()


{
	progress->setStyleSheet("QProgressBar::chunk {background-color: lime;}");
	this->timer = new QTimer(this);
	int timeout = 2000;
	connect(timer, SIGNAL(timeout()), this, SLOT(remove()));
	timer->start(timeout);
	info->setText("Operation succeeded");

}

void medMessageControllerMessageProgress::abortMessage()


{
	progress->setStyleSheet("QProgressBar::chunk {background-color: red;}");
	this->timer = new QTimer(this);
	int timeout = 2000;
	connect(timer, SIGNAL(timeout()), this, SLOT(remove()));
	timer->start(timeout);
	info->setText("Operation aborted");

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

void medMessageController::showInfo(const QString& text,unsigned int timeout)
{
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) {
        // GUI
        medMessageControllerMessageInfo *message = new medMessageControllerMessageInfo(
                text,0,timeout);

        emit addMessage(message->getWidget());

    } else {
        dtkTrace() << text;
    }
}

void medMessageController::showError(const QString& text,unsigned int timeout)
{
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) {
        // GUI
        medMessageControllerMessageError *message = new medMessageControllerMessageError(
                text,0,timeout);

        emit addMessage(message->getWidget());

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

void medMessageController::success(QObject *sender)
{
	medMessageControllerMessage *message = d->messages.value(sender);
	if(medMessageControllerMessageProgress *progress = dynamic_cast<medMessageControllerMessageProgress *>(message))
	{
		progress->successMessage();
	}
	else remove(message);
}

void medMessageController::failure(QObject *sender)
{
	medMessageControllerMessage *message = d->messages.value(sender);
	if(medMessageControllerMessageProgress *progress = dynamic_cast<medMessageControllerMessageProgress *>(message))
	{
		progress->abortMessage();
	}
	else remove(message);
}


void medMessageController::remove(QObject *sender)
{
    medMessageControllerMessage *message = d->messages.value(sender);
    if(message != NULL)
    {
        emit removeMessage(message->getWidget());
        d->messages.remove(sender);
        delete message;
    }
}

void medMessageController::remove(medMessageControllerMessage *message){
	if(message != NULL){
		emit removeMessage(message->getWidget());
		delete message;
	}
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
