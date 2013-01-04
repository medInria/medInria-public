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

medMessageControllerMessage::medMessageControllerMessage(
                                                         QWidget *parent,
														 const QString& text, 
														 unsigned int timeout, 
														 bool messageProgress) : QWidget(parent)
{
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

    this->setLayout(layout);
}

medMessageControllerMessage::~medMessageControllerMessage(void)
{

}

void medMessageControllerMessage::remove(){
	medMessageController::instance()->remove(this);
}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageInfo
// /////////////////////////////////////////////////////////////////

medMessageControllerMessageInfo::medMessageControllerMessageInfo(
        const QString& text, QWidget *parent,unsigned int timeout) :
        medMessageControllerMessage(parent,text, timeout)
{
    icon->setPixmap(QPixmap(":/icons/information.png"));
	this->setFixedWidth(200);
}

medMessageControllerMessageInfo::~medMessageControllerMessageInfo(void)
{

}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageError
// /////////////////////////////////////////////////////////////////

medMessageControllerMessageError::medMessageControllerMessageError(
        const QString& text, QWidget *parent,unsigned int timeout) :
        medMessageControllerMessage(parent, text, timeout)
{
    icon->setPixmap(QPixmap(":/icons/exclamation.png"));
	this->setFixedWidth(300);
}

medMessageControllerMessageError::~medMessageControllerMessageError(void)
{

}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageProgress
// /////////////////////////////////////////////////////////////////


medMessageControllerMessageProgress::medMessageControllerMessageProgress(
        const QString& text, QWidget *parent) :
        medMessageControllerMessage(parent, text, 0, true)
{
}

medMessageControllerMessageProgress::~medMessageControllerMessageProgress(void)
{
}

void medMessageControllerMessageProgress::setProgress(int value)
{
    progress->setValue(value);
	progress->update();
	qApp->processEvents();
}

void medMessageControllerMessageProgress::success(void)


{
	progress->setStyleSheet("QProgressBar::chunk {background-color: lime;}");
	this->timer = new QTimer(this);
	int timeout = 2000;
	connect(timer, SIGNAL(timeout()), this, SLOT(remove()));
	timer->start(timeout);
	info->setText("Operation succeeded");

}

void medMessageControllerMessageProgress::failure(void)


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

medMessageControllerMessageQuestion::medMessageControllerMessageQuestion(const QString& text, QWidget *parent) : medMessageControllerMessage(parent), d(new medMessageControllerMessageQuestionPrivate)
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

    this->setLayout(layout);
}

medMessageControllerMessageQuestion::~medMessageControllerMessageQuestion(void)
{
    delete d;
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
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) {
        // GUI
        medMessageControllerMessageInfo *message = new medMessageControllerMessageInfo(
                text,0,timeout);

        emit addMessage(message);

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

        emit addMessage(message);

    } else {
        dtkError() << text;
    }
}

medMessageControllerMessageProgress * medMessageController::showProgress(const QString& text)
{
    if ( dynamic_cast<QApplication *>(QCoreApplication::instance()) ) {
        // GUI
        medMessageControllerMessageProgress *message = new medMessageControllerMessageProgress(text);

        emit addMessage(message);
		return message;

    } /*else {
        dtkDebug() << "Progress : " << text;
    }*/

}


void medMessageController::remove(medMessageControllerMessage *message){
	if(message != NULL){
		emit removeMessage(message);
		delete message;
	}
}

medMessageController::medMessageController(void) : QObject()
{
}

medMessageController::~medMessageController(void)
{
}

medMessageController *medMessageController::s_instance = NULL;
