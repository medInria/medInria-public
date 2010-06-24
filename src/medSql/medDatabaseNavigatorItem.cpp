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

#include <QtCore>
#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItem
// /////////////////////////////////////////////////////////////////

// -- TODO
//
// Passing database indexes as arguments to the constructor is more than
// sufficiant.
//
// Attributes such as path to thumbnail, count and text should be retreived
// from the database.

class medDatabaseNavigatorItemPrivate
{
public:
    int patientId;
    int studyId;
    int seriesId;
    int imageId;

    QString path;
};

medDatabaseNavigatorItem::medDatabaseNavigatorItem(int patientId, int studyId, int seriesId, int imageId, const QString &path, QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabaseNavigatorItemPrivate)
{
    d->patientId = patientId;
    d->studyId = studyId;
    d->seriesId = seriesId;
    d->imageId = imageId;

    d->path = path;

    medDatabaseNavigatorItemLoader *loader = new medDatabaseNavigatorItemLoader(path);

    connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));

    QThreadPool::globalInstance()->start(loader);

    this->setAcceptHoverEvents(true);
}

medDatabaseNavigatorItem::medDatabaseNavigatorItem(int patientId, int studyId, int seriesId, int imageId, const QImage &image, QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabaseNavigatorItemPrivate)
{
    d->patientId = patientId;
    d->studyId = studyId;
    d->seriesId = seriesId;
    d->imageId = imageId;

    d->path = QString();

    this->setImage(image);
    this->setAcceptHoverEvents(true);
}

medDatabaseNavigatorItem::~medDatabaseNavigatorItem(void)
{
    delete d;

    d = NULL;
}

medDatabaseNavigatorItem *medDatabaseNavigatorItem::clone(void)
{
    return new medDatabaseNavigatorItem(d->patientId, d->studyId, d->seriesId, d->imageId, d->path);
}

void medDatabaseNavigatorItem::setup(void)
{
    // Retrieve attributes from the database
}

int medDatabaseNavigatorItem::patientId(void) const
{
    return d->patientId;
}

int medDatabaseNavigatorItem::studyId(void) const
{
    return d->studyId;
}

int medDatabaseNavigatorItem::seriesId(void) const
{
    return d->seriesId;
}

int medDatabaseNavigatorItem::imageId(void) const
{
    return d->imageId;
}

QString medDatabaseNavigatorItem::path(void) const
{
    return d->path;
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
    QString index = QString("%1:%2:%3:%4").arg(d->patientId).arg(d->studyId).arg(d->seriesId).arg(d->imageId);

    QMimeData *data = new QMimeData;
    data->setData("med/index", index.toLatin1());
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
