/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabaseNavigatorItem.h"
#include <medDatabaseNavigatorItemOverlay.h>
#include "medImageFileLoader.h"

#include <medCore/medAbstractDbController.h>
#include <medCore/medDataManager.h>
#include <medCore/medMetaDataKeys.h>

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
    
    medDatabaseNavigatorItemOverlay *itemSaver;
    medDatabaseNavigatorItemOverlay *itemExporter;
    medDatabaseNavigatorItemOverlay *itemTrasher;
    
    QPropertyAnimation *itemSaverFadingAnimation;
    QPropertyAnimation *itemExporterFadingAnimation;
    QPropertyAnimation *itemTrasherFadingAnimation;
    
    QParallelAnimationGroup *fadingAnimation;
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
    
    d->itemExporter = new medDatabaseNavigatorItemOverlay(this);
    d->itemExporter->setToolTip("<span style=\"background: #fff8dc;\">Export data to disk</span>");
    QPixmap pixmap(":/icons/export.png");
    d->itemExporter->setPixmap(pixmap);
    d->itemExporter->setPos(10, 45);
    d->itemExporter->setOpacity(0.0);
    
    connect(d->itemExporter, SIGNAL(clicked()), this, SLOT(exportData()));
    
    // Setup animation        
    d->itemExporterFadingAnimation = new QPropertyAnimation(d->itemExporter, "opacity", this);
    d->itemExporterFadingAnimation->setDuration(150);
    
    d->fadingAnimation = new QParallelAnimationGroup(this);
    d->fadingAnimation->addAnimation(d->itemExporterFadingAnimation);
    
    if (!d->persistent)
    {
        d->itemSaver = new medDatabaseNavigatorItemOverlay(this);
        QPixmap pixmapSave(":/icons/import.png");
        d->itemSaver->setToolTip("<span style=\"background: #fff8dc;\">Save data to database</span>");
        d->itemSaver->setPixmap(pixmapSave);
        d->itemSaver->setPos(10, 65);
        d->itemSaver->setOpacity(0.0);
        
        connect(d->itemSaver, SIGNAL(clicked()), this, SLOT(saveData()));
        
        // Setup animation        
        d->itemSaverFadingAnimation = new QPropertyAnimation(d->itemSaver, "opacity", this);
        d->itemSaverFadingAnimation->setDuration(150);
        
        d->fadingAnimation->addAnimation(d->itemSaverFadingAnimation);
    }
    else
    {
        d->itemSaver = NULL;
        d->itemSaverFadingAnimation = NULL;
    }
    
    d->itemTrasher = new medDatabaseNavigatorItemOverlay(this);
    d->itemTrasher->setToolTip("<span style=\"background: #fff8dc;\">Remove data</span>");
    QPixmap pixmapDelete(":/icons/cross.svg");
    d->itemTrasher->setPixmap(pixmapDelete.scaledToWidth(16));
    d->itemTrasher->setPos(10, 25);
    d->itemTrasher->setOpacity(0.0);
    
    connect(d->itemTrasher, SIGNAL(clicked()), this, SLOT(deleteData()));
    
    // Setup animation        
    d->itemTrasherFadingAnimation = new QPropertyAnimation(d->itemTrasher, "opacity", this);
    d->itemTrasherFadingAnimation->setDuration(150);
    
    d->fadingAnimation->addAnimation(d->itemTrasherFadingAnimation);

    connect(this, SIGNAL(onItemDoubleClicked(const medDataIndex&)), medDataManager::instance(), SIGNAL(onItemDoubleClicked(const medDataIndex&)));
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
    int reply = QMessageBox::question(NULL, tr("Remove item"),
                                      tr("Are you sure you want to continue removing this data?\n""This cannot be undone."),
                                      QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton);
    
    if( reply == QMessageBox::Yes )
        medDataManager::instance()->removeData(d->index);
}

void medDatabaseNavigatorItem::saveData()
{
    medDataManager::instance()->storeNonPersistentSingleDataToDatabase(d->index);
}

void medDatabaseNavigatorItem::exportData()
{
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

    medDataManager::instance()->exportDataToFile(data, fileName);
}

void medDatabaseNavigatorItem::setImage(const QImage& image)
{
    this->setPixmap(QPixmap::fromImage(image));
}

void medDatabaseNavigatorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void medDatabaseNavigatorItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    emit onItemDoubleClicked(d->index);
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
    if (d->itemSaverFadingAnimation)
    {
        d->itemSaverFadingAnimation->setStartValue(0.0);
        d->itemSaverFadingAnimation->setEndValue(1.0);
    }

    d->itemExporterFadingAnimation->setStartValue(0.0);
    d->itemExporterFadingAnimation->setEndValue(1.0);

    d->itemTrasherFadingAnimation->setStartValue(0.0);
    d->itemTrasherFadingAnimation->setEndValue(1.0);

    d->fadingAnimation->start();
}

void medDatabaseNavigatorItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (d->itemSaverFadingAnimation)
    {
        d->itemSaverFadingAnimation->setStartValue(1.0);
        d->itemSaverFadingAnimation->setEndValue(0.0);
    }
    
    d->itemExporterFadingAnimation->setStartValue(1.0);
    d->itemExporterFadingAnimation->setEndValue(0.0);
    
    d->itemTrasherFadingAnimation->setStartValue(1.0);
    d->itemTrasherFadingAnimation->setEndValue(0.0);
    
    d->fadingAnimation->start();
}
