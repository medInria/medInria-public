/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabasePreviewItem.h>
#include <medImageFileLoader.h>

#include <medDataIndex.h>
#include <medAbstractDbController.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItemPrivate
{
public:
    medDataIndex index;
    int slice;  // Slice number of the item
    bool isDraggingAllowed;
};

medDatabasePreviewItem::medDatabasePreviewItem(const medDataIndex &index, QGraphicsItem *parent) : QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabasePreviewItemPrivate)
{
    d->index = index;
    d->slice = 0;
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index,medMetaDataKeys::ThumbnailPath);

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
    connect(this, SIGNAL(openRequested(const medDataIndex&, int)), medDataManager::instance(), SIGNAL(openRequested(const medDataIndex&, int)));
    // we allow dragging by default;
    d->isDraggingAllowed = true;
}

medDatabasePreviewItem::~medDatabasePreviewItem(void)
{
    delete d;

    d = NULL;
}

medDatabasePreviewItem *medDatabasePreviewItem::clone(void)
{
    return new medDatabasePreviewItem(d->index);
}

medDataIndex medDatabasePreviewItem::dataIndex(void) const
{
    return d->index;
}

void medDatabasePreviewItem::setImage(const QImage& image)
{
    this->setPixmap(QPixmap::fromImage(image));
}

void medDatabasePreviewItem::setAllowDragging(bool isDraggingAllowed)
{
    d->isDraggingAllowed = isDraggingAllowed;
}

void medDatabasePreviewItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
}

void medDatabasePreviewItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    emit openRequested(d->index, d->slice);
}

void medDatabasePreviewItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(!d->isDraggingAllowed)
        return;

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

void medDatabasePreviewItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    emit hoverEntered(event, this);
}

void medDatabasePreviewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    emit hoverLeft(event, this);
}

/**
* @brief Allow to specify the slice number of the item
* @param nb - slice number
*/
void medDatabasePreviewItem::setSlice(int nb)
{
    d->slice = nb;
}
