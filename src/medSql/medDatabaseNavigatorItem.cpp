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
#include "medDatabaseNavigatorItemLoader.h"

#include <medCore/medAbstractDbController.h>
#include <medCore/medDataManager.h>
#include <medCore/medMetaDataHelper.h>

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

    QString text;
};

medDatabaseNavigatorItem::medDatabaseNavigatorItem(const medDataIndex & index,  QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabaseNavigatorItemPrivate)
{
    d->index = index;

    // d->text = text; 
    //, thumbPath, seriesName.toString());

    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData( index, medMetaDataHelper::KEY_ThumbnailPath() );

    d->text = dbc->metaData( index, medMetaDataHelper::KEY_SeriesDescription() );

    if ( thumbpath.isEmpty() ) {
        this->setImage( dbc->thumbnail(index) ) ;
    } else {
        medDatabaseNavigatorItemLoader *loader = new medDatabaseNavigatorItemLoader(thumbpath);

        connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));

        QThreadPool::globalInstance()->start(loader);
    }

    this->setAcceptHoverEvents(true);
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
    QGraphicsPixmapItem::paint (painter, option, widget);

    painter->setPen(Qt::white);
    painter->drawText(option->rect.adjusted(5, 5, -5, option->rect.height()-5), Qt::AlignRight | Qt::TextSingleLine, d->text);
}
