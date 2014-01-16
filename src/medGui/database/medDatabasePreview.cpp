#include "medDatabasePreview.h"

#include "medDataIndex.h"
#include "medAbstractDbController.h"
#include "medDataManager.h"
#include "medImageFileLoader.h"


#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>

#include "medDatabaseThumbnailHelper.h"



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
    d->baseWidth = medDatabaseThumbnailHelper::width;
    d->baseHeight = medDatabaseThumbnailHelper::height;

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
    d->currentDataIndex = index;
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index, medMetaDataKeys::ThumbnailPath);

    QImage thumbnailImg;

    if (!thumbpath.isEmpty())
        thumbnailImg = QImage(thumbpath);
    else
        thumbnailImg = dbc->thumbnail(index);

    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    this->addItem(pixmap);

    if(!thumbnailImg.isNull())
        pixmap->setPixmap(QPixmap::fromImage(thumbnailImg));
    else
        pixmap->setPixmap(QPixmap(":/medGui/pixmaps/default_thumbnail.png"));
}

// medDatabasePreviewStudyScene
void medDatabasePreviewStaticScene::addImage(const medDataIndex &index)
{
    d->isMulti = true;

    int nbItem = this->items().size();
    if(nbItem > 5)
        return;

    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index, medMetaDataKeys::ThumbnailPath);

    QImage thumbnailImg;

    if (!thumbpath.isEmpty())
        thumbnailImg = QImage(thumbpath);
    else
        thumbnailImg = dbc->thumbnail(index);

    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    this->addItem(pixmap);

    if(!thumbnailImg.isNull())
        pixmap->setPixmap(QPixmap::fromImage(thumbnailImg));
    else
        pixmap->setPixmap(QPixmap(":/medGui/pixmaps/default_thumbnail.png"));


    switch(nbItem)
    {
    case 1:
    {
        this->addLine(d->baseWidth + d->pen.width()/2,
                0,
                d->baseWidth + d->pen.width()/2,
                (d->baseHeight  + d->pen.width()/2) * 2,
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
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(d->currentDataIndex.dataSourceId());
        QString thumbpath = dbc->metaData(d->currentDataIndex, medMetaDataKeys::ThumbnailPath);

        QImage thumbnailImg;

        if (!thumbpath.isEmpty())
            thumbnailImg = QImage(thumbpath);
        else
            thumbnailImg = dbc->thumbnail(d->currentDataIndex);


        QPixmap pixmap;
        if(!thumbnailImg.isNull())
            pixmap = QPixmap::fromImage(thumbnailImg);
        else
            pixmap = QPixmap(":/medGui/pixmaps/default_thumbnail.png");

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
    QMap <QString, medDataIndex> seriesDescriptionDataIndexMap;
    QList <QString> seriesDescriptions;
};

medDatabasePreviewDynamicScene::medDatabasePreviewDynamicScene(QMap<QString, medDataIndex> &seriesDescriptionDataIndexMap,
                                                           QObject *parent):
    d(new medDatabasePreviewDynamicScenePrivate)
{
    d->seriesDescriptionDataIndexMap = seriesDescriptionDataIndexMap;
    d->seriesDescriptions = d->seriesDescriptionDataIndexMap.keys();
    qSort(d->seriesDescriptions.begin(), d->seriesDescriptions.end());
}

medDatabasePreviewDynamicScene::~medDatabasePreviewDynamicScene()
{
    delete d;
    d = NULL;
}


void medDatabasePreviewDynamicScene::previewMouseMoveEvent(QMouseEvent *event, int width)
{
    int seriesIndex = event->x() / (width / d->seriesDescriptions.size());
    if(seriesIndex < d->seriesDescriptions.size())
    {
        this->setImage(d->seriesDescriptionDataIndexMap.value(d->seriesDescriptions[seriesIndex]));
    }
}


// medDatabasePreview
class medDatabasePreviewPrivate
{
public:
    medDatabasePreview::medDataType currentDataType;
    medDatabasePreviewStaticScene *staticScene;
    medDatabasePreviewDynamicScene *dynamicScene;

    QLabel *label;

};


medDatabasePreview::medDatabasePreview(QWidget *parent): d(new medDatabasePreviewPrivate)
{
    this->setMouseTracking(true);

    QGraphicsScene *scene = new QGraphicsScene;
    this->setScene(scene);

    d->dynamicScene = NULL;
    d->staticScene = NULL;

    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    pixmap->setPixmap(QPixmap(":/medGui/pixmaps/default_thumbnail.png"));
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
    QMap <QString, medDataIndex> seriesDescriptionDataIndexMap;

    foreach (medDataIndex serieIndex, seriesIndex)
    {
        d->staticScene->addImage(serieIndex);
        QString seriesDescription = dbc->metaData(serieIndex, medMetaDataKeys::SeriesDescription);
        seriesDescriptionDataIndexMap.insert(seriesDescription, serieIndex);
    }

    d->dynamicScene = new medDatabasePreviewDynamicScene(seriesDescriptionDataIndexMap);
    connect(d->dynamicScene, SIGNAL(openRequest(medDataIndex)), this, SIGNAL(openRequest(medDataIndex)));


    QString itemDescription = dbc->metaData(index, medMetaDataKeys::StudyDescription);
    d->label->setText(itemDescription);
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
