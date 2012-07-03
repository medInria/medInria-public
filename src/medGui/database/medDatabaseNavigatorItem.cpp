/* medDatabaseNavigatorItem.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:35 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu May 13 20:12:12 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 25
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include "medDatabaseNavigatorItem.h"
#include <medDatabaseNavigatorItemOverlay.h>
#include "medImageFileLoader.h"

#include <medCore/medAbstractDbController.h>
#include <medCore/medDataManager.h>
#include <medCore/medMetaDataKeys.h>

#include <medSql/medDatabaseExporter.h>

#include <QtCore>
#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItem
// /////////////////////////////////////////////////////////////////

// -- TODO
//
// Passing database indexes as arguments to the constructor is more than
// sufficient.
//
// Attributes such as path to thumbnail, count and text should be retrieved
// from the database.

class medDatabaseNavigatorItemPrivate
{
public:
    medDataIndex index;
    bool persistent;
    QString text;
    
    medDatabaseNavigatorItemOverlay *item_saver;
    medDatabaseNavigatorItemOverlay *item_exporter;
    medDatabaseNavigatorItemOverlay *item_trasher;
    
    QPropertyAnimation *item_saver_fading_animation;
    QPropertyAnimation *item_exporter_fading_animation;
    QPropertyAnimation *item_trasher_fading_animation;
    
    QParallelAnimationGroup *fading_animation;
};

medDatabaseNavigatorItem::medDatabaseNavigatorItem(const medDataIndex & index,  QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabaseNavigatorItemPrivate)
{
    d->index = index;

    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index,medMetaDataKeys::ThumbnailPath);

    d->persistent = dbc->isPersistent();
    d->text = dbc->metaData(index,medMetaDataKeys::SeriesDescription);


    bool shouldSkipLoading = false;
    if ( thumbpath.isEmpty() ) {
        // first try to get it from controller
        QImage thumbImage = dbc->thumbnail(index);
        if (!thumbImage.isNull())
        {
            this->setImage( thumbImage );
            shouldSkipLoading = true;
        }
    }

    if (!shouldSkipLoading) {
        medImageFileLoader *loader = new medImageFileLoader(thumbpath);
        connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));
        QThreadPool::globalInstance()->start(loader);
    }

    this->setAcceptHoverEvents(true);
    
    d->item_exporter = new medDatabaseNavigatorItemOverlay(this);
    QPixmap pixmap(":/icons/export.png");
    d->item_exporter->setPixmap(pixmap);
    d->item_exporter->setPos(10, 45);
    d->item_exporter->setOpacity(0.0);
    
    connect(d->item_exporter, SIGNAL(clicked()), this, SLOT(exportData()));
    
    // Setup animation        
    d->item_exporter_fading_animation = new QPropertyAnimation(d->item_exporter, "opacity", this);
    d->item_exporter_fading_animation->setDuration(150);
    
    d->fading_animation = new QParallelAnimationGroup(this);
    d->fading_animation->addAnimation(d->item_exporter_fading_animation);    
    
    if (!d->persistent)
    {
        d->item_saver = new medDatabaseNavigatorItemOverlay(this);
        QPixmap pixmapSave(":/icons/import.png");
        d->item_saver->setPixmap(pixmapSave);
        d->item_saver->setPos(10, 65);
        d->item_saver->setOpacity(0.0);
        
        connect(d->item_saver, SIGNAL(clicked()), this, SLOT(saveData()));
        
        // Setup animation        
        d->item_saver_fading_animation = new QPropertyAnimation(d->item_saver, "opacity", this);
        d->item_saver_fading_animation->setDuration(150);
        
        d->fading_animation->addAnimation(d->item_saver_fading_animation);
    }
    else
    {
        d->item_saver = NULL;
        d->item_saver_fading_animation = NULL;
    }
    
    d->item_trasher = new medDatabaseNavigatorItemOverlay(this);
    QPixmap pixmapDelete(":/icons/cross.svg");
    d->item_trasher->setPixmap(pixmapDelete.scaledToWidth(16));
    d->item_trasher->setPos(10, 25);
    d->item_trasher->setOpacity(0.0);
    
    connect(d->item_trasher, SIGNAL(clicked()), this, SLOT(deleteData()));
    
    // Setup animation        
    d->item_trasher_fading_animation = new QPropertyAnimation(d->item_trasher, "opacity", this);
    d->item_trasher_fading_animation->setDuration(150);
    
    d->fading_animation->addAnimation(d->item_trasher_fading_animation);
}

medDatabaseNavigatorItem::~medDatabaseNavigatorItem(void)
{
    delete d;

    d = NULL;
}

medDatabaseNavigatorItem *medDatabaseNavigatorItem::clone(void)
{
    return new medDatabaseNavigatorItem(d->index, 0);
}

void medDatabaseNavigatorItem::setup(void)
{
    // Retrieve attributes from the database
}

medDataIndex medDatabaseNavigatorItem::dataIndex(void) const
{
    return d->index;
}

QString medDatabaseNavigatorItem::text(void) const
{
    return d->text;
}

void medDatabaseNavigatorItem::deleteData()
{
    medDataManager::instance()->removeData(d->index);
}

void medDatabaseNavigatorItem::saveData()
{
    medDataManager::instance()->storeNonPersistentSingleDataToDatabase(d->index);
}

void medDatabaseNavigatorItem::exportData()
{
    //medDataManager::instance()->exportDataFromDatabase(d->index);
    
    QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save as"), "", "*.*");
    
    if (fileName.isEmpty())
        return;
    
    dtkSmartPointer<dtkAbstractData> data = medDataManager::instance()->data(d->index);
    
    if (!data)
        return;
    
    //Check extension:
    QFileInfo fileInfo(fileName);
    if(fileInfo.suffix().isEmpty())
    {
        qDebug() << "determining suffix for type" << data->identifier();
        if (data->identifier().contains("vtk") ||
            data->identifier().contains("v3d"))
            fileName.append(".vtk");
        else
            fileName.append(".nii.gz");
        qDebug() << "filename:" << fileName;
        //There may be other cases, but this will get us through most
    }
    
    medDatabaseExporter *exporter = new medDatabaseExporter (data, fileName);
    
    QThreadPool::globalInstance()->start(exporter);
}

void medDatabaseNavigatorItem::setImage(const QImage& image)
{
    this->setPixmap(QPixmap::fromImage(image));
}

void medDatabaseNavigatorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void medDatabaseNavigatorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QMimeData *data = d->index.createMimeData();
    data->setImageData(this->pixmap());

	QDrag *drag = new QDrag(this->scene()->views().first());
    drag->setMimeData(data);
#if WIN32
	drag->setPixmap(this->pixmap().scaled(64,64));
#else
    drag->setPixmap(this->pixmap());
#endif
    drag->setHotSpot(QPoint(drag->pixmap().width()/2, drag->pixmap().height()/2));
    drag->start();
}

void medDatabaseNavigatorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(d->persistent)
        painter->setBrush(Qt::darkGray);
    else
        painter->setBrush(QColor(129, 143, 164));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(option->rect.adjusted(-8,-8, 8, 8),5,5);

    QGraphicsPixmapItem::paint (painter, option, widget);

    painter->setPen(Qt::white);
    painter->drawText(option->rect.adjusted(5, 5, -5, option->rect.height()-5), Qt::AlignRight | Qt::TextSingleLine, d->text);
}

void medDatabaseNavigatorItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (d->item_saver_fading_animation)
    {
        d->item_saver_fading_animation->setStartValue(0.0);
        d->item_saver_fading_animation->setEndValue(1.0);
    }

    d->item_exporter_fading_animation->setStartValue(0.0);
    d->item_exporter_fading_animation->setEndValue(1.0);

    d->item_trasher_fading_animation->setStartValue(0.0);
    d->item_trasher_fading_animation->setEndValue(1.0);

    d->fading_animation->start();
}

void medDatabaseNavigatorItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (d->item_saver_fading_animation)
    {
        d->item_saver_fading_animation->setStartValue(1.0);
        d->item_saver_fading_animation->setEndValue(0.0);
    }
    
    d->item_exporter_fading_animation->setStartValue(1.0);
    d->item_exporter_fading_animation->setEndValue(0.0);
    
    d->item_trasher_fading_animation->setStartValue(1.0);
    d->item_trasher_fading_animation->setEndValue(0.0);
    
    d->fading_animation->start();
}
