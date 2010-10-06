/* medPacsWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 11:07:29 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 12:29:49 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 197
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
#include <medPacs/medAbstractPacsNode.h>
#include <medPacs/medAbstractPacsResultDataset.h>

class medPacsWidgetPrivate
{
public:
    QString host_title;
    QString host_address;
    QString host_port;

    QList<QStringList> nodes;

    medAbstractPacsFindScu *find;
};

medPacsWidget::medPacsWidget(QWidget *parent) : QTreeWidget(parent), d(new medPacsWidgetPrivate)
{
    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setUniformRowHeights(true);
    this->setAlternatingRowColors(true);
    this->setSortingEnabled(true);

    this->setHeaderLabels(QStringList() << "Name" << "Description" << "Id" << "Modality");

    d->find = medAbstractPacsFactory::instance()->createFindScu("dcmtkFindScu");

    this->readSettings();

    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(onItemClicked(QTreeWidgetItem *, int)));
}

medPacsWidget::~medPacsWidget(void)
{
    this->writeSettings();

    delete d;

    d = NULL;
}

void medPacsWidget::readSettings(void)
{
    QSettings settings("inria", "medinria");
    settings.beginGroup("medToolBoxPacsHost");
    d->host_title = settings.value("title").toString();
    d->host_address = settings.value("address").toString();
    d->host_port = settings.value("port").toString();
    settings.endGroup();

    QList<QVariant> nodes;

    settings.beginGroup("medToolBoxPacsNodes");
    nodes = settings.value("nodes").toList();
    settings.endGroup();

    d->nodes.clear();

    foreach(QVariant node, nodes)
        d->nodes << node.toStringList();
}

void medPacsWidget::writeSettings(void)
{

}

void medPacsWidget::search(QString query)
{
    this->readSettings();

    this->clear();

    if(d->find) {
        
        d->find->clearAllQueryAttributes();
        d->find->setQueryLevel(medAbstractPacsFindScu::STUDY);
        d->find->addQueryAttribute(0x0010,0x0010, query.toAscii().constData());   // patient name
        d->find->addQueryAttribute(0x0008,0x0030, "\0"); // study date
        d->find->addQueryAttribute(0x0008,0x0050, "\0"); // accession no
        d->find->addQueryAttribute(0x0008,0x0061, "\0"); // modalities in study
        d->find->addQueryAttribute(0x0008,0x0090, "\0"); // ref physician
        d->find->addQueryAttribute(0x0008,0x1030, "\0"); // study description
        d->find->addQueryAttribute(0x0010,0x0020, "\0"); // patient ID
        d->find->addQueryAttribute(0x0010,0x0030, "\0"); // patient BD
        d->find->addQueryAttribute(0x0010,0x0040, "\0"); // sex
        d->find->addQueryAttribute(0x0020,0x000D, "\0"); // studyInstanceUID
        d->find->addQueryAttribute(0x0020,0x0010, "\0"); // study ID
        
        foreach(QStringList node, d->nodes)
            d->find->sendFindRequest(node.at(0).toLatin1(), node.at(1).toLatin1(), node.at(2).toInt(), d->host_title.toLatin1(), d->host_address.toLatin1(), d->host_port.toInt());
        
        QVector<medAbstractPacsNode *> nodes = d->find->getNodeContainer(); int i = 0;
        
        foreach(medAbstractPacsNode *node, nodes) {
            
            QVector<medAbstractPacsResultDataset*> container = node->getResultDatasetContainer();
            
            foreach(medAbstractPacsResultDataset *dataset, container) {
                
                QTreeWidgetItem *item = new QTreeWidgetItem(this, QStringList()
                                                            << QString(dataset->findKeyValue(0x0010,0x0010))
                                                            << QString(dataset->findKeyValue(0x0008,0x1030))
                                                            << QString(dataset->findKeyValue(0x0020,0x0010))
                                                            << QString(dataset->findKeyValue(0x0008,0x0061)));
                
                item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
                item->setData(0,Qt::UserRole, i++);
                item->setData(1,Qt::UserRole, QPoint(0x0020,0x000D));
                item->setData(2,Qt::UserRole, QString(dataset->getStudyInstanceUID()));
            }
        }
    }
}

void medPacsWidget::onItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    if(!item->parent() && !item->childCount()) 
        this->findSeriesLevel(item);

    if(item->parent() && !item->childCount())
        this->findImageLevel(item);
}

void medPacsWidget::findSeriesLevel(QTreeWidgetItem * item)
{
    this->readSettings();

    int nodeIndex = item->data(0,Qt::UserRole).toInt();

    QPoint tag = item->data(1,Qt::UserRole).toPoint();
    QString searchStr = item->data(2,Qt::UserRole).toString();

    d->find->clearAllQueryAttributes();
    d->find->setQueryLevel(medAbstractPacsFindScu::SERIES);
    d->find->addQueryAttribute(tag.x(),tag.y(),searchStr.toLatin1()); // studyInstanceUID
    d->find->addQueryAttribute(0x0008,0x0021,"\0"); // series date
    d->find->addQueryAttribute(0x0008,0x0031,"\0"); // series time
    d->find->addQueryAttribute(0x0008,0x0060,"\0"); // series modality
    d->find->addQueryAttribute(0x0008,0x103E,"\0"); // series description
    d->find->addQueryAttribute(0x0018,0x0015,"\0"); // body part
    d->find->addQueryAttribute(0x0018,0x1030,"\0"); // protocol name
    d->find->addQueryAttribute(0x0018,0x5100,"\0"); // patient position
    d->find->addQueryAttribute(0x0020,0x000E,"\0"); // series instance UID
    d->find->addQueryAttribute(0x0020,0x0011,"\0"); // series number
    d->find->addQueryAttribute(0x0020,0x0052,"\0"); // frame of reference

    foreach(QStringList node, d->nodes)
        d->find->sendFindRequest(node.at(0).toLatin1(), node.at(1).toLatin1(), node.at(2).toInt(), d->host_title.toLatin1(), d->host_address.toLatin1(), d->host_port.toInt());
    
    QVector<medAbstractPacsNode *> nodes = d->find->getNodeContainer();
    
    foreach(medAbstractPacsNode *node, nodes) {
        
        QVector<medAbstractPacsResultDataset*> container = node->getResultDatasetContainer();
        
        foreach(medAbstractPacsResultDataset *dataset, container) {
            
            QTreeWidgetItem *pItem = new QTreeWidgetItem(item);
            pItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
            pItem->setData(0,Qt::UserRole, nodeIndex);
            pItem->setData(1,Qt::UserRole, QPoint(0x0020,0x000E));
            pItem->setData(2,Qt::UserRole, QString(dataset->getSeriesInstanceUID()));
            pItem->setText(2,dataset->findKeyValue(0x0020,0x0011));
            item->addChild(pItem);
        }
    }
}

void medPacsWidget::findImageLevel(QTreeWidgetItem *item)
{
    this->readSettings();

    int nodeIndex = item->data(0,Qt::UserRole).toInt();

    QString searchStr = item->data(2,Qt::UserRole).toString();

    d->find->clearAllQueryAttributes();
    d->find->setQueryLevel(medAbstractPacsFindScu::IMAGE);
    d->find->addQueryAttribute(0x0020,0x000E,searchStr.toLatin1()); // series instance UID
    d->find->addQueryAttribute(0x0008,0x0008,"\0"); // image type
    d->find->addQueryAttribute(0x0008,0x0012,"\0"); // instance creation date
    d->find->addQueryAttribute(0x0008,0x0013,"\0"); // instance creation time
    d->find->addQueryAttribute(0x0008,0x0016,"\0"); // SOP class UID
    d->find->addQueryAttribute(0x0008,0x0018,"\0"); // SOP instance UID
    d->find->addQueryAttribute(0x0008,0x0022,"\0"); // image date
    d->find->addQueryAttribute(0x0008,0x0032,"\0"); // image time
    d->find->addQueryAttribute(0x0020,0x0012,"\0"); // acquisition number
    d->find->addQueryAttribute(0x0020,0x000D,"\0"); // study instance UID
    d->find->addQueryAttribute(0x0020,0x0013,"\0"); // instance time
    d->find->addQueryAttribute(0x0020,0x0032,"\0"); // image position patient
    d->find->addQueryAttribute(0x0020,0x0037,"\0"); // image orientation patient
    d->find->addQueryAttribute(0x0020,0x1041,"\0"); // slice location
    d->find->addQueryAttribute(0x0028,0x0008,"\0"); // number of frames

    foreach(QStringList node, d->nodes)
        d->find->sendFindRequest(node.at(0).toLatin1(), node.at(1).toLatin1(), node.at(2).toInt(), d->host_title.toLatin1(), d->host_address.toLatin1(), d->host_port.toInt());

    QVector<medAbstractPacsNode *> nodes = d->find->getNodeContainer();
    
    foreach(medAbstractPacsNode *node, nodes) {
        
        QVector<medAbstractPacsResultDataset*> container = node->getResultDatasetContainer();
        
        foreach(medAbstractPacsResultDataset *dataset, container) {
            
            QTreeWidgetItem *pItem = new QTreeWidgetItem(item);
            
            pItem->setData(0,Qt::UserRole, nodeIndex);
            pItem->setData(1,Qt::UserRole, QPoint(0x0008,0x0018));
            pItem->setData(2,Qt::UserRole, QString(dataset->getSOPInstanceUID()));
            
            pItem->setText(2, dataset->findKeyValue(0x0020,0x0012));
            pItem->setText(3, dataset->findKeyValue(0x0008,0x0008));
            
            item->addChild(pItem);
        }
    }
}
