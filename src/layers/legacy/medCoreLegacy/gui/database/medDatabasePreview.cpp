/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabasePreview.h"

#include <medDataIndex.h>
#include <medAbstractDbController.h>
#include <medDatabaseController.h>
#include <medDatabaseNonPersistentController.h>
#include <medDataManager.h>
#include <medGlobalDefs.h>

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QDrag>
#include <QPixmap>
#include <QMouseEvent>
#include <QLabel>

class medDatabasePreviewStaticScenePrivate
{
public:
    int baseWidth;
    int baseHeight;
    QPen pen;
    bool isMulti;
    medDataIndex currentDataIndex;
};


medDatabasePreviewStaticScene::medDatabasePreviewStaticScene(QObject *parent):
    d(new medDatabasePreviewStaticScenePrivate)
{
    d->baseWidth = med::defaultThumbnailSize.width();
    d->baseHeight = med::defaultThumbnailSize.height();

    d->pen = QPen(QColor(70,70,70));
    d->pen.setWidth(4);

    d->isMulti = false;
}

medDatabasePreviewStaticScene::~medDatabasePreviewStaticScene()
{
    delete d;
    d = NULL;
}

void medDatabasePreviewStaticScene::setImage(const medDataIndex &index)
{
    this->clear();
    d->currentDataIndex = index;

    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    pixmap->setPixmap(medDataManager::instance()->thumbnail(index));
    this->addItem(pixmap);

    if( ! this->views().isEmpty()) {
        foreach(QGraphicsView * v, this->views()) {
            v->fitInView(pixmap, Qt::KeepAspectRatio);
        }
    }

}

// medDatabasePreviewStudyScene
void medDatabasePreviewStaticScene::addImage(const medDataIndex &index)
{
    d->isMulti = true;

    int nbItem = this->items().size();
    if(nbItem > 5)
        return;

    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    pixmap->setPixmap(medDataManager::instance()->thumbnail(index));
    QTransform transfo;
    transfo=transfo.scale(d->baseWidth / pixmap->boundingRect().width(),
                  d->baseHeight / pixmap->boundingRect().height());
    pixmap->setTransform(transfo);
    this->addItem(pixmap);

    switch(nbItem)
    {
        case 1:
        {
            this->addLine(d->baseWidth + d->pen.width()/2,
                          0,
                          d->baseWidth + d->pen.width()/2,
                          (d->baseHeight + d->pen.width()/2) * 2,
                          d->pen
                          );
            this->addLine(0,
                          d->baseHeight + d->pen.width()/2,
                          (d->baseWidth + d->pen.width()/2) * 2,
                          d->baseHeight + d->pen.width()/2,
                          d->pen
                          );

            pixmap->moveBy(d->baseWidth + d->pen.width() + 1,
                           0
                           );
            break;
        }
        case 4:
        {
            pixmap->moveBy(0,
                           d->baseHeight + d->pen.width() +1
                           );
            break;
        }
        case 5:
        {
            pixmap->moveBy(d->baseWidth + d->pen.width() +1,
                           d->baseHeight + d->pen.width() + 1
                           );
            break;
        }
        default:
            break;
    }
}

medDataIndex& medDatabasePreviewStaticScene::currentDataIndex() const
{
    return d->currentDataIndex;
}


void medDatabasePreviewStaticScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(!d->isMulti)
        emit openRequest(d->currentDataIndex);

    QGraphicsScene::mouseDoubleClickEvent(event);
}

void medDatabasePreviewStaticScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!d->isMulti && event->buttons() == Qt::LeftButton)
    {
        QPixmap pixmap = medDataManager::instance()->thumbnail(d->currentDataIndex);

        QMimeData *data = d->currentDataIndex.createMimeData();
        data->setImageData(pixmap);

        QDrag *drag = new QDrag(this->views().first());
        drag->setMimeData(data);
        drag->setPixmap(pixmap);

        drag->setHotSpot(QPoint(drag->pixmap().width()/2, drag->pixmap().height()/2));
        drag->start();
    }
    QGraphicsScene::mouseMoveEvent(event);
}

// medDatabasePreviewStudyScene
class medDatabasePreviewDynamicScenePrivate
{
public:
    QList<QPair<medDataIndex,QString> > seriesDescriptionDataIndexPairList;
};

bool stringMedDataIndexPairLessThan(const QPair<medDataIndex,QString> & a,
                                    const QPair<medDataIndex,QString> & b) {
    return (a.second < b.second);
}

medDatabasePreviewDynamicScene::medDatabasePreviewDynamicScene(const QList<QPair<medDataIndex,QString> > & seriesDescriptionDataIndexList,
                                                               QObject * parent):
    d(new medDatabasePreviewDynamicScenePrivate)
{
    d->seriesDescriptionDataIndexPairList = seriesDescriptionDataIndexList;
    qSort(d->seriesDescriptionDataIndexPairList.begin(), d->seriesDescriptionDataIndexPairList.end(), &stringMedDataIndexPairLessThan);
}

medDatabasePreviewDynamicScene::~medDatabasePreviewDynamicScene()
{
    delete d;
    d = NULL;
}


void medDatabasePreviewDynamicScene::previewMouseMoveEvent(QMouseEvent *event, int width)
{
    if(d->seriesDescriptionDataIndexPairList.empty())
        return;

    int seriesIndex = event->x() / (width / (double)d->seriesDescriptionDataIndexPairList.size());
    seriesIndex = qBound(0, seriesIndex, d->seriesDescriptionDataIndexPairList.size()-1);
    this->setImage(d->seriesDescriptionDataIndexPairList.at(seriesIndex).first);
    emit updateLabel(d->seriesDescriptionDataIndexPairList.at(seriesIndex).second);
}


// medDatabasePreview
class medDatabasePreviewPrivate
{
public:
    medDatabasePreview::medDataType currentDataType;
    medDatabasePreviewStaticScene *staticScene;
    medDatabasePreviewDynamicScene *dynamicScene;

    QLabel *label;
    QString defaultText;
};


medDatabasePreview::medDatabasePreview(QWidget *parent): d(new medDatabasePreviewPrivate)
{
    this->setMouseTracking(true);

    QGraphicsScene *scene = new QGraphicsScene;
    this->setScene(scene);

    d->dynamicScene = NULL;
    d->staticScene = NULL;

    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    pixmap->setPixmap(QPixmap(":/pixmaps/default_thumbnail.png"));
    this->fitInView(pixmap, Qt::KeepAspectRatio);
    scene->addItem(pixmap);

    d->label = new QLabel(this);
    d->label->setAlignment(Qt::AlignCenter);
    d->label->setObjectName("previewLabel");
    d->label->setText("Nothing selected");

    this->setMinimumSize(184, 184);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


medDatabasePreview::~medDatabasePreview()
{
    delete d;
    d = NULL;
}


void medDatabasePreview::resizeEvent(QResizeEvent *event)
{
    this->resize(this->width(), this->width());
    this->fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(event);
}

void medDatabasePreview::showSeriesPreview(const medDataIndex &index)
{
    d->currentDataType = medDatabasePreview::SERIES;
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());

    if (d->staticScene)
        delete d->staticScene;
    d->staticScene = new medDatabasePreviewStaticScene;
    connect(d->staticScene, SIGNAL(openRequest(medDataIndex)), this, SIGNAL(openRequest(medDataIndex)));

    d->staticScene->setImage(index);

    QString itemDescription = dbc->metaData(index, medMetaDataKeys::SeriesDescription);
    d->label->setText(itemDescription);

    this->setScene(d->staticScene);
    this->fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
}


void medDatabasePreview::showStudyPreview(const medDataIndex &index)
{
    d->currentDataType = medDatabasePreview::STUDY;
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());

    if (d->staticScene)
        delete d->staticScene;
    if (d->dynamicScene)
        delete d->dynamicScene;

    d->staticScene = new medDatabasePreviewStaticScene;
    connect(d->staticScene, SIGNAL(openRequest(medDataIndex)), this, SIGNAL(openRequest(medDataIndex)));

    this->setScene(d->staticScene);

    QList<medDataIndex> seriesIndex = dbc->series(index);
    QList<QPair<medDataIndex,QString> > seriesDescriptionDataIndexPairList;

    foreach (medDataIndex serieIndex, seriesIndex)
    {
        d->staticScene->addImage(serieIndex);
        QString seriesDescription = dbc->metaData(serieIndex, medMetaDataKeys::SeriesDescription);
        seriesDescriptionDataIndexPairList.append(qMakePair(serieIndex, seriesDescription));
    }

    d->dynamicScene = new medDatabasePreviewDynamicScene(seriesDescriptionDataIndexPairList);
    connect(d->dynamicScene, SIGNAL(openRequest(medDataIndex)), this, SIGNAL(openRequest(medDataIndex)));
    connect(d->dynamicScene, SIGNAL(updateLabel(QString)), this, SLOT(setLabel(QString)));


    QString itemDescription = dbc->metaData(index, medMetaDataKeys::StudyDescription);
    d->label->setText(itemDescription);
    d->defaultText = itemDescription;
    this->fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
}



void medDatabasePreview::showPatientPreview(const medDataIndex &index)
{
    d->currentDataType = medDatabasePreview::PATIENT;
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());

    if (d->staticScene)
        delete d->staticScene;
    d->staticScene = new medDatabasePreviewStaticScene;
    connect(d->staticScene, SIGNAL(openRequest(medDataIndex)), this, SIGNAL(openRequest(medDataIndex)));

    this->setScene(d->staticScene);

    d->staticScene->setImage(index);

    QString itemDescription = dbc->metaData(index, medMetaDataKeys::PatientName);
    d->label->setText(itemDescription);
    this->fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void medDatabasePreview::setLabel(const QString &text)
{
    d->label->setText(text);
}


QLabel* medDatabasePreview::label() const
{
    return d->label;
}

void medDatabasePreview::enterEvent(QEvent *event)
{
    switch(d->currentDataType)
    {
        case medDatabasePreview::SERIES:
        {
            break;
        }
        case medDatabasePreview::STUDY:
        {
            this->setScene(d->dynamicScene);
            this->fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
            break;
        }
        case medDatabasePreview::PATIENT:
        {
            break;
        }
    }
    QGraphicsView::enterEvent(event);

}


void medDatabasePreview::leaveEvent(QEvent *event)
{
    switch(d->currentDataType)
    {
        case medDatabasePreview::SERIES:
        {
            break;
        }
        case medDatabasePreview::STUDY:
        {
            this->setScene(d->staticScene);
            this->fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
            this->setLabel(d->defaultText);
            break;
        }
        case medDatabasePreview::PATIENT:
        {
            break;
        }
    }
    QGraphicsView::leaveEvent(event);
}

void medDatabasePreview::mouseMoveEvent(QMouseEvent *event)
{
    switch(d->currentDataType)
    {
        case medDatabasePreview::SERIES:
        {
            break;
        }
        case medDatabasePreview::STUDY:
        {
            if (event->buttons() == Qt::LeftButton)
                break;
            d->dynamicScene->previewMouseMoveEvent(event, this->width());
            this->fitInView(this->scene()->sceneRect(), Qt::KeepAspectRatio);
            break;
        }
        case medDatabasePreview::PATIENT:
        {
            break;
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}
