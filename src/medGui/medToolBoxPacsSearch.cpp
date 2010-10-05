/* medToolBoxPacsSearch.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 15:49:05 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct  5 19:04:58 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 113
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxPacsSearch.h"

#include <medPacs/medAbstractPacsFactory.h>
#include <medPacs/medAbstractPacsFindScu.h>
#include <medPacs/medAbstractPacsNode.h>
#include <medPacs/medAbstractPacsResultDataset.h>

#include <QtGui>

class medToolBoxPacsSearchPrivate
{
public:
    QLineEdit *edit;
    QPushButton *button;
};

medToolBoxPacsSearch::medToolBoxPacsSearch(QWidget *parent) : medToolBox(parent), d(new medToolBoxPacsSearchPrivate)
{
    QWidget *page = new QWidget(this);

    d->edit = new QLineEdit(page);
    d->button = new QPushButton("Search", page);

    QHBoxLayout *layout = new QHBoxLayout(page);
    layout->addWidget(d->edit);
    layout->addWidget(d->button);

    this->setTitle("Pacs search");
    this->setWidget(page);

    connect(d->button, SIGNAL(clicked()), this, SLOT(onSearch()));
}

medToolBoxPacsSearch::~medToolBoxPacsSearch(void)
{
    delete d;

    d = NULL;
}

void medToolBoxPacsSearch::onSearch(void)
{
    medAbstractPacsFindScu *scu = medAbstractPacsFactory::instance()->createFindScu("dcmtkFindScu");

    scu->setQueryLevel(medAbstractPacsFindScu::STUDY);
    scu->addQueryAttribute(0x0010,0x0010, d->edit->text().toAscii().constData());   // patient name
    scu->addQueryAttribute(0x0008,0x0030, "\0"); // study date
    scu->addQueryAttribute(0x0008,0x0050, "\0"); // accession no
    scu->addQueryAttribute(0x0008,0x0061, "\0"); // modalities in study
    scu->addQueryAttribute(0x0008,0x0090, "\0"); // ref physician
    scu->addQueryAttribute(0x0008,0x1030, "\0"); // study description
    scu->addQueryAttribute(0x0010,0x0020, "\0"); // patient ID
    scu->addQueryAttribute(0x0010,0x0030, "\0"); // patient BD
    scu->addQueryAttribute(0x0010,0x0040, "\0"); // sex
    scu->addQueryAttribute(0x0020,0x000D, "\0"); // studyInstanceUID
    scu->addQueryAttribute(0x0020,0x0010, "\0"); // study ID

    if(!scu->sendFindRequest("DCM4CHEE","jumbo-4.irisa.fr", 10012, "medinria", "localhost", 9999)) {

        QVector<medAbstractPacsNode *> nodes = scu->getNodeContainer();

        foreach(medAbstractPacsNode *node, nodes) {
            
            qDebug() << "Getting find results for node" << QString::fromStdString(node->title());

            QVector<medAbstractPacsResultDataset*> container = node->getResultDatasetContainer();

            foreach(medAbstractPacsResultDataset *dataset, container) {
                
                qDebug() << "study uid:" << dataset->getStudyInstanceUID();
                qDebug() << "instance uid:" << dataset->getSeriesInstanceUID();
                qDebug() << "sop uid:" << dataset->getSOPInstanceUID();

                
            }
        }
    }
}
