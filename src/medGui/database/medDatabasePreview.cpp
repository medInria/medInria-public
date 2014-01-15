#include "medDatabasePreview.h"

#include "medDataIndex.h"
#include "medAbstractDbController.h"
#include "medDataManager.h"
#include "medImageFileLoader.h"


#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>

#include "medDatabaseThumbnailHelper.h"

class medDatabasePreviewPrivate
{
public:
    QGraphicsScene *scene;
    QLabel *label;
};


medDatabasePreview::medDatabasePreview(QWidget *parent): d(new medDatabasePreviewPrivate)
{
    d->scene = new QGraphicsScene;
    this->setScene(d->scene);

    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    pixmap->setPixmap(QPixmap(":/medGui/pixmaps/default_thumbnail.png"));
    this->fitInView(pixmap, Qt::KeepAspectRatio);
    d->scene->addItem(pixmap);

    d->label = new QLabel(this);
    d->label->setAlignment(Qt::AlignCenter);
    d->label->setObjectName("previewLabel");
    d->label->setText("No selesction");

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
    this->fitInView(d->scene->sceneRect(), Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(event);
}

void medDatabasePreview::showSeriesThumbnail(const medDataIndex &index)
{

    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index, medMetaDataKeys::ThumbnailPath);
    if (!thumbpath.isEmpty())
        this->setImage(QImage(thumbpath));
    else
        this->setImage(dbc->thumbnail(index));

    QString itemDescription = dbc->metaData(index, medMetaDataKeys::SeriesDescription);
    d->label->setText(itemDescription);
}


void medDatabasePreview::showStudyThumbnail(const medDataIndex &index)
{
    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());


    QList<medDataIndex> series = dbc->series(index);
    if(series.size() < 4)
    {
        QString thumbpath = dbc->metaData(series[0], medMetaDataKeys::ThumbnailPath);
        if (!thumbpath.isEmpty())
            this->setImage(QImage(thumbpath));
        else
            this->setImage(dbc->thumbnail(series[0]));
    }
    else
    {
        setImage(QImage());
    }
    QString itemDescription = dbc->metaData(index, medMetaDataKeys::StudyDescription);
    d->label->setText(itemDescription);
}



void medDatabasePreview::showPatientThumbnail(const medDataIndex &index)
{

    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index, medMetaDataKeys::ThumbnailPath);
    if (!thumbpath.isEmpty())
        this->setImage(QImage(thumbpath));
    else
        this->setImage(dbc->thumbnail(index));


    QString itemDescription = dbc->metaData(index, medMetaDataKeys::PatientName);
    d->label->setText(itemDescription);
}



void medDatabasePreview::setImage(const QImage &image)
{
    scene()->clear();
    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    d->scene->addItem(pixmap);

    if(!image.isNull())
        pixmap->setPixmap(QPixmap::fromImage(image));
    else
        pixmap->setPixmap(QPixmap(":/medGui/pixmaps/default_thumbnail.png"));

    this->fitInView(d->scene->sceneRect(), Qt::KeepAspectRatio);
}

void medDatabasePreview::addImage(const QImage &image)
{
    scene()->clear();
    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    d->scene->addItem(pixmap);
    pixmap->setPixmap(QPixmap(":/medGui/pixmaps/default_thumbnail.png"));
    this->fitInView(pixmap, Qt::KeepAspectRatio);
}

QLabel* medDatabasePreview::label()
{
    return d->label;
}


