/* medPacsWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 11:07:29 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 18:57:16 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 343
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
#include <medPacs/medAbstractPacsStoreScp.h>
#include <medPacs/medAbstractPacsResultDataset.h>

#include <QUuid>

#include <dtkCore/dtkGlobal.h>

// /////////////////////////////////////////////////////////////////
// medPacsWidgetPrivate
// /////////////////////////////////////////////////////////////////

class medPacsWidgetPrivate : public QThread
{
protected:
    void run(void);

public:
    int index(medAbstractPacsNode& node);

public:
    QString host_title;
    QString host_address;
    QString host_port;

    QList<QStringList> nodes;
    QList<QStringList> selectedNodes;

    medAbstractPacsFindScu  *find;
    medAbstractPacsEchoScu  *echo;
    medAbstractPacsStoreScp *server;

};

void medPacsWidgetPrivate::run(void)
{
    if(!this->server) {
        qDebug() << "Unable to find a valid implementation of the store scp service.";
        return;
    }
    QDir tmp = QDir::temp();
    tmp.mkdir("import");
    tmp.cd("import");
    this->server->setStorageDirectory(tmp.absolutePath().toLatin1());
    this->server->start(this->host_title.toLatin1(), this->host_address.toLatin1(), tryToInt(this->host_port));
}

int medPacsWidgetPrivate::index(medAbstractPacsNode& node)
{
    for(int i = 0; i < this->nodes.count(); i++)
    {
        QString port;
        port.setNum(node.port());
        if( (this->nodes[i].contains(node.title())) && (this->nodes[i].contains(node.ip())) && (this->nodes[i].contains(port)) )
            return i;
    }

    return -1;
}


// /////////////////////////////////////////////////////////////////
// medPacsWidget
// /////////////////////////////////////////////////////////////////

medPacsWidget::medPacsWidget(QWidget *parent) : QTreeWidget(parent), d(new medPacsWidgetPrivate)
{
    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setUniformRowHeights(true);
    this->setAlternatingRowColors(true);
    this->setSortingEnabled(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setSelectionMode(QAbstractItemView::ExtendedSelection);

    this->setHeaderLabels(QStringList() << "Name" << "Description" << "Id" << "Modality");

    d->find = NULL;
    d->echo = NULL;
    d->server = medAbstractPacsFactory::instance()->createStoreScp("dcmtkStoreScp");
    if (!d->server) qDebug() << "Warning server could not be started, pacsmodule not loaded.";

    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(onItemExpanded(QTreeWidgetItem *)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(updateContextMenu(const QPoint&)));
    connect(this->d->server, SIGNAL(endOfStudy(QString)), this, SIGNAL(import(QString)));
    
    this->readSettings();
    d->start();
    d->selectedNodes = d->nodes;

}

medPacsWidget::~medPacsWidget(void)
{
    if (d->find) delete d->find;
    if (d->echo) delete d->echo;
    if (d->isRunning())
    {
        d->server->stop();
        d->exit();
        d->wait();
    }
    delete d;

    d = NULL;
}

void medPacsWidget::readSettings(void)
{
    QSettings settings("inria", "medinria");
    settings.beginGroup("medBrowserToolBoxPacsHost");
    QString title = settings.value("title").toString();
    d->host_address = "localhost";
    QString port = settings.value("port").toString();
    settings.endGroup();

    if (title.isEmpty())
        d->host_title = (tr("MEDINRIA"));
    else
        d->host_title = title;
    if (port.isEmpty())
        d->host_port = tr("9999");
    else
        d->host_port = port;

    QList<QVariant> nodes;

    settings.beginGroup("medBrowserToolBoxPacsNodes");
    nodes = settings.value("nodes").toList();
    settings.endGroup();

    d->nodes.clear();

    foreach(QVariant node, nodes)
        d->nodes << node.toStringList();

}


void medPacsWidget::search(QString query)
{
    this->readSettings();

    this->clear();

    if (!d->find) d->find = medAbstractPacsFactory::instance()->createFindScu("dcmtkFindScu");
    if(d->find) 
    {
        d->find->clearAllQueryAttributes();
        d->find->setQueryLevel(medAbstractPacsFindScu::STUDY);
        d->find->addQueryAttribute(0x0010,0x0010, query.toAscii().constData()); // patient name
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
        
        foreach(QStringList node, d->selectedNodes)
            d->find->sendFindRequest(node.at(0).toLatin1(), node.at(1).toLatin1(), tryToInt(node.at(2)), 
                d->host_title.toLatin1(), d->host_address.toLatin1(), tryToInt(d->host_port));
        
        QVector<medAbstractPacsNode *> nodes = d->find->getNodeContainer();
        
        foreach(medAbstractPacsNode *node, nodes) {
            
            QVector<medAbstractPacsResultDataset *> container = node->getResultDatasetContainer();
            
            foreach(medAbstractPacsResultDataset *dataset, container) {
                
                QTreeWidgetItem *item = new QTreeWidgetItem(this, QStringList()
                                                            << QString(dataset->findKeyValue(0x0010,0x0010))
                                                            << QString(dataset->findKeyValue(0x0008,0x1030))
                                                            << QString(dataset->findKeyValue(0x0020,0x0010))
                                                            << QString(dataset->findKeyValue(0x0008,0x0061)));
                
                item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
                item->setData(0,Qt::UserRole, d->index(*node));
                item->setData(1,Qt::UserRole, QPoint(0x0020,0x000D));
                item->setData(2,Qt::UserRole, QString(dataset->getStudyInstanceUID()));
            }
        }
    }else {
        qDebug() << "findScu: cannot create instance, maybe module was not loaded?";
    }
}

void medPacsWidget::onItemExpanded(QTreeWidgetItem *item)
{
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


    for (int i=0; i<d->nodes.count();i++)
    {
        if (i == nodeIndex)
        {
            QStringList node = d->nodes.at(i);
            d->find->sendFindRequest(node.at(0).toLatin1(), node.at(1).toLatin1(), tryToInt(node.at(2)), 
                d->host_title.toLatin1(), d->host_address.toLatin1(), tryToInt(d->host_port));
        }
    }

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

    for (int i=0; i<d->nodes.count();i++)
    {
        if (i == nodeIndex)
        {
            QStringList node = d->nodes.at(i);
            d->find->sendFindRequest(node.at(0).toLatin1(), node.at(1).toLatin1(), tryToInt(node.at(2)), 
                  d->host_title.toLatin1(), d->host_address.toLatin1(), tryToInt(d->host_port));
        }
    }

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

void medPacsWidget::updateContextMenu(const QPoint& point)
{
    QModelIndex index = this->indexAt(point);

    if(!index.isValid())
        return;

    QTreeWidgetItem *item = itemFromIndex(index);

    QMenu menu(this);
    menu.addAction("Import", this, SLOT(onItemImported()));
    menu.exec(mapToGlobal(point));
}



void medPacsWidget::onItemImported(void)
{
    this->readSettings();
    QList<QTreeWidgetItem*> itemList = this->selectedItems();

    QHash<int,int>hash;
    for (int i = 0; i < itemList.size(); ++i) 
    {
        if(!(hash.contains(itemList.at(i)->data(0, Qt::UserRole).toInt())))
            hash.insert(itemList.at(i)->data(0, Qt::UserRole).toInt(),i);
    }

    for(int u = 0; u<hash.size();u++)
    {

    QVector<medMoveCommandItem> cmdList;
        for (int i = 0; i < itemList.size(); ++i) 
        {
            int nodeIndex = itemList.at(i)->data(0, Qt::UserRole).toInt();
            QPoint tag    = itemList.at(i)->data(1, Qt::UserRole).toPoint();
            QString query = itemList.at(i)->data(2, Qt::UserRole).toString();

            medMoveCommandItem item;
            item.group = tag.x();
            item.elem  = tag.y();
            item.sourceTitle = d->nodes.at(nodeIndex).at(0);
            item.sourceIp = d->nodes.at(nodeIndex).at(1);
            item.sourcePort = tryToInt(d->nodes.at(nodeIndex).at(2));
            item.targetTitle = d->host_title;
            item.targetIp = d->host_address;
            item.targetPort = tryToInt(d->host_port);
            item.query = query;

            if(hash.contains(itemList.at(i)->data(0, Qt::UserRole).toInt()))
                cmdList.push_back(item);
        }
        emit moveList(cmdList);
    }
}

void medPacsWidget::updateSelectedNodes( QVector<int> list )
{
    readSettings();
    d->selectedNodes.clear();
    for(int i=0; i<list.count(); i++)
    {
        d->selectedNodes.push_back(d->nodes.at(list.at(i)));
    }
}

void medPacsWidget::onEchoRequest()
{
    this->readSettings();
    QVector<bool> response;

    foreach(QStringList node, d->nodes)
    {
        if(!d->echo) d->echo = medAbstractPacsFactory::instance()->createEchoScu("dcmtkEchoScu");
        if(d->echo)
        {
            if(!d->echo->sendEchoRequest(node.at(0).toLatin1(), node.at(1).toLatin1(), tryToInt(node.at(2)), 
                    d->host_title.toLatin1(), d->host_address.toLatin1(),tryToInt(d->host_port) ))
                response.push_back(true);
            else
                response.push_back(false);

        } else {
            qDebug() << "echoScu: cannot create instance, maybe module was not loaded?";
        }
    }

    emit echoResponse(response);
}

int tryToInt( QString value )
{
    int result = 0;
    bool ok;
    try
    {
        result = value.toInt(&ok,10);
        if(!ok) result = 0;
    }
    catch (...)
    {
        result = 0;
    }
    return result;
}
