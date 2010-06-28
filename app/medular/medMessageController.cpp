/* medMessageController.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jun 28 09:59:08 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Jun 28 16:28:53 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 107
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medBrowserArea.h"
#include "medBrowserArea_p.h"
#include "medMessageController.h"
#include "medViewerArea.h"
// #include "medViewerArea_p.h"

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessage
// /////////////////////////////////////////////////////////////////

medMessageControllerMessage::medMessageControllerMessage(QWidget *parent) : QWidget(parent)
{
    this->setFixedWidth(400);
}

medMessageControllerMessage::~medMessageControllerMessage(void)
{

}

// /////////////////////////////////////////////////////////////////
// medMessageControllerMessageInfo
// /////////////////////////////////////////////////////////////////

medMessageControllerMessageInfo::medMessageControllerMessageInfo(const QString& text, QWidget *parent) : medMessageControllerMessage(parent)
{
    QLabel *icon = new QLabel(this);
    icon->setPixmap(QPixmap(":/icons/information.png"));

    QLabel *info = new QLabel(this);
    info->setText(text);
    
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignLeft);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(icon);
    layout->addWidget(info);
}

medMessageControllerMessageInfo::~medMessageControllerMessageInfo(void)
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

medMessageControllerMessageProgress::medMessageControllerMessageProgress(const QString& text, QWidget *parent) : medMessageControllerMessage(parent), d(new medMessageControllerMessageProgressPrivate)
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

}

void medMessageControllerMessageProgress::setProgress(int value)
{
    d->progress->setValue(value);
}

// /////////////////////////////////////////////////////////////////
// medMessageController
// /////////////////////////////////////////////////////////////////

class medMessageControllerPrivate
{
public:
    medBrowserArea *browser;
    medViewerArea *viewer;

    QHash<int, medMessageControllerMessage *> messages;
};

medMessageController *medMessageController::instance(void)
{
    if(!s_instance)
        s_instance = new medMessageController;

    return s_instance;
}

void medMessageController::attach(medBrowserArea *browser)
{
    d->browser = browser;
}

void medMessageController::attach(medViewerArea *viewer)
{
    d->viewer = viewer;
}

int medMessageController::showInfo(const QString& text)
{
    medMessageControllerMessageInfo *message = new medMessageControllerMessageInfo(text);
    
    d->browser->d->status->addWidget(message);
    d->browser->d->status->update();
    qApp->processEvents();

    d->messages.insert(d->messages.count(), message);

    return d->messages.count()-1;
}

int medMessageController::showProgress(const QString& text)
{
    medMessageControllerMessageProgress *message = new medMessageControllerMessageProgress(text);
    
    d->browser->d->status->addWidget(message);
    d->browser->d->status->update();
    qApp->processEvents();

    d->messages.insert(d->messages.count(), message);

    return d->messages.count()-1;
}

void medMessageController::setProgress(int id, int value)
{
    medMessageControllerMessage *message = d->messages.value(id);

    if(medMessageControllerMessageProgress *progress = dynamic_cast<medMessageControllerMessageProgress *>(message)) {
        progress->setProgress(value);
        progress->update();
        qApp->processEvents();
    }
}

void medMessageController::remove(int id)
{
    medMessageControllerMessage *message = d->messages.value(id);

    d->browser->d->status->removeWidget(message);

    d->messages.remove(id);

    delete message;
}

medMessageController::medMessageController(void) : QObject(), d(new medMessageControllerPrivate)
{
    d->browser = NULL;
    d->viewer = NULL;
}

medMessageController::~medMessageController(void)
{
    delete d;

    d = NULL;
}

medMessageController *medMessageController::s_instance = NULL;
