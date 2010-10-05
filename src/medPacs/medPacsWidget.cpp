/* medPacsWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 11:07:29 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct  5 17:25:52 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 58
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medPacsWidget.h"

#include <medPacs/medAbstractPacsFactory.h>
#include <medPacs/medAbstractPacsFindScu.h>
#include <medPacs/medAbstractPacsEchoScu.h>

class medPacsWidgetPrivate
{
public:
    QString host_title;
    QString host_address;
    QString host_port;

    QString node_title;
    QString node_address;
    QString node_port;
};

medPacsWidget::medPacsWidget(QWidget *parent) : QTreeWidget(parent), d(new medPacsWidgetPrivate)
{
    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setUniformRowHeights(true);
    this->setAlternatingRowColors(true);
    this->setSortingEnabled(true);

    this->setHeaderLabels(QStringList() << "Name" << "Description" << "Id" << "Modality");

    // d->find = medAbstractPacsFactory::instance()->createFindScu("dcmtkFindScu");
}

medPacsWidget::~medPacsWidget(void)
{
    delete d;

    d = NULL;
}

void medPacsWidget::setHostTitle(const QString& title)
{
    d->host_title = title;
}

void medPacsWidget::setHostAddress(const QString& address)
{
    d->host_address = address;
}

void medPacsWidget::setHostPort(const QString& port)
{
    d->host_port = port;
}

void medPacsWidget::setNodeTitle(const QString& title)
{
    d->node_title = title;
}

void medPacsWidget::setNodeAddress(const QString& address)
{
    d->node_address = address;
}

void medPacsWidget::setNodePort(const QString& port)
{
    d->node_port = port;
}

// /////////////////////////////////////////////////////////////////
// 
// /////////////////////////////////////////////////////////////////

void medPacsWidget::echoTest(void)
{
    qDebug() << "factory -> createEchoScu";

    medAbstractPacsEchoScu *echoScu = medAbstractPacsFactory::instance()->createEchoScu("dcmtkEchoScu");

    if (echoScu != NULL) {
        try
        {
            qDebug() << "Calling echo function of plugin";


            if(!echoScu->sendEchoRequest("DCM4CHEE","jumbo-4.irisa.fr", 10012, "medinria", "localhost", 9999))
                qDebug() << "Echo success";
            else
                qDebug() << "Echo failure";
        }
        catch (...)
        {
            qDebug() << "exception thrown";
        }

    } else {
        qDebug() << "Unable to create ECHOSCU";
    }
}


void medPacsWidget::findTest()
{
    qDebug() << "factory -> createFindScu";

    medAbstractPacsFindScu *findScu = medAbstractPacsFactory::instance()->createFindScu("dcmtkFindScu");
    if (findScu != NULL) 
    {
        try
        {
            qDebug() << "Calling find function of plugin";
            findScu->setQueryLevel(medAbstractPacsFindScu::STUDY);
            findScu->addQueryAttribute(0x0010,0x0010,""); // patient name
            findScu->addQueryAttribute(0x0008,0x0030,"\0"); // study date
            findScu->addQueryAttribute(0x0008,0x0050,"\0"); // accession no
            findScu->addQueryAttribute(0x0008,0x0061,"\0"); // modalities in study
            findScu->addQueryAttribute(0x0008,0x0090,"\0"); // ref physician
            findScu->addQueryAttribute(0x0008,0x1030,"\0"); // study description
            findScu->addQueryAttribute(0x0010,0x0020,"\0"); // patient ID
            findScu->addQueryAttribute(0x0010,0x0030,"\0"); // patient BD
            findScu->addQueryAttribute(0x0010,0x0040,"\0"); // sex
            findScu->addQueryAttribute(0x0020,0x000D,"\0"); // studyInstanceUID
            findScu->addQueryAttribute(0x0020,0x0010,"\0"); // study ID

            if(!findScu->sendFindRequest("DCM4CHEE","jumbo-4.irisa.fr", 10012, "medinria", "localhost", 9999))
                qDebug() << "Find success";
            else
                qDebug() << "Find failure";

            findScu->clearAllQueryAttributes();
        }
        catch (...)
        {
            qDebug() << "exception thrown";
        }
    }
    else qDebug() << "Unable to create FINDSCU";
}
