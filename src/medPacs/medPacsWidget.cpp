#include "medPacsWidget.h"

#include <medPacs/medAbstractPacsFactory.h>
#include <medPacs/medAbstractPacsFindScu.h>
#include <medPacs/medAbstractPacsEchoScu.h>


medPacsWidget::medPacsWidget( QWidget* parent ) : QTreeView(parent)
{
    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setUniformRowHeights(true);
    this->setAlternatingRowColors(true);
    this->setSortingEnabled(true);
}

medPacsWidget::~medPacsWidget()
{

}

void medPacsWidget::echoTest()
{
    qDebug() << "factory -> createEchoScu";
    medAbstractPacsEchoScu *echoScu = medAbstractPacsFactory::instance()->createEchoScu("dcmtkEchoScu");
    if (echoScu != NULL) 
    {
        try
        {
            qDebug() << "Calling echo function of plugin";
            echoScu->sendEchoRequest("CONQUESTSRV1","localhost",5678,"dcmtkTool","localhost",100);
        }
        catch (...)
        {
            qDebug() << "exception thrown";
        }

    }
    else qDebug() << "Unable to create ECHOSCU";
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
            findScu->sendFindRequest("CONQUESTSRV1","localhost",5678,"dcmtkTool","localhost",100);
            findScu->clearAllQueryAttributes();
        }
        catch (...)
        {
            qDebug() << "exception thrown";
        }
    }
    else qDebug() << "Unable to create FINDSCU";
}
