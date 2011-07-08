/* medApplication.cpp ---
 *
 * Author: John Stark
 * Copyright (C) 2011 - John Stark, Inria.
 * Created: May 2011
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medApplication.h"

#include <QtGui>

#include <dtkCore/dtkGlobal.h>
#include <dtkCore/dtkLog.h>
#include <medCore/medPluginManager.h>

class medApplicationPrivate
{
public:
    QColor msgColor;
    int msgAlignment;
};

// /////////////////////////////////////////////////////////////////
// medApplication
// /////////////////////////////////////////////////////////////////

medApplication::medApplication(int & argc, char**argv) :
        QApplication(argc,argv),
        d(new medApplicationPrivate)
{
    this->setApplicationName("medinria");
    this->setApplicationVersion("0.0.1");
    this->setOrganizationName("inria");
    this->setOrganizationDomain("fr");
    this->setWindowIcon(QIcon(":/medinria.ico"));

    //The ownership of the style object is transferred.
    this->setStyle( new QPlastiqueStyle() );
    // this->setStyleSheet(dtkReadFile(":/medinria.qss")); -> must be done in medMainWindow

    //Set some splash screen properties:
    setMsgColor(Qt::black);
    setMsgAlignment(Qt::AlignLeft);

    //redirect medPluginManager msgs to the logs
    QObject::connect(medPluginManager::instance(), SIGNAL(loadError(const QString &)),
                     this, SLOT(redirectErrorMessageToLog(const QString&)) );
    QObject::connect(medPluginManager::instance(), SIGNAL(loaded(QString)),
                     this, SLOT(redirectMessageToLog(QString)) );
}

medApplication::~medApplication(void)
{
    delete d;
    d = NULL;
}

void medApplication::setMsgColor(const QColor& color)
{
    d->msgColor = color;
}

const QColor& medApplication::msgColor()
{
    return d->msgColor;
}

int medApplication::msgAlignment()
{
    return d->msgAlignment;
}

void medApplication::setMsgAlignment(int alignment)
{
    d->msgAlignment = alignment;
}

void medApplication::redirectMessageToLog(const QString &message)
{
    dtkOutput()<< message;
}

void medApplication::redirectErrorMessageToLog(const QString &message)
{
    dtkError()<< message;
}

void medApplication::redirectMessageToSplash(const QString &message)
{
    emit showMessage(message,d->msgAlignment,d->msgColor);
}
