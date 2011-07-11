/* medDatabasePreviewItem.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:43:00 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:43:01 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabasePreviewItem.h"
#include "medDatabasePreviewItemLoader.h"

#include <medCore/medDataIndex.h>
#include <medCore/medAbstractDbController.h>
#include <medCore/medDataManager.h>
#include <medCore/medMetaDataHelper.h>

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItemPrivate
{
public:
    medDataIndex index;
};

medDatabasePreviewItem::medDatabasePreviewItem(const medDataIndex &index, QGraphicsItem *parent) : QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabasePreviewItemPrivate)
{
    d->index = index;
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData( index, medMetaDataHelper::KEY_ThumbnailPath() );

    if ( thumbpath.isEmpty() ) {
        this->setImage( dbc->thumbnail(index) ) ;
    } else {
        medDatabasePreviewItemLoader *loader = new medDatabasePreviewItemLoader(thumbpath);

        connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));

        QThreadPool::globalInstance()->start(loader);
    }
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
