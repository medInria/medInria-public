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
    delete d->scene;
    d->scene = new QGraphicsScene;
    this->setScene(d->scene);

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
    delete d->scene;
    d->scene = new QGraphicsScene;
    this->setScene(d->scene);

    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());


    QList<medDataIndex> series = dbc->series(index);

    foreach (medDataIndex serie, series)
    {
        QString thumbpath = dbc->metaData(serie, medMetaDataKeys::ThumbnailPath);
        if (!thumbpath.isEmpty())
            this->addImage(QImage(thumbpath));
        else
            this->addImage(dbc->thumbnail(serie));
    }

    QString itemDescription = dbc->metaData(index, medMetaDataKeys::StudyDescription);
    d->label->setText(itemDescription);
}



void medDatabasePreview::showPatientThumbnail(const medDataIndex &index)
{
    delete d->scene;
    d->scene = new QGraphicsScene;
    this->setScene(d->scene);

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
    QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem;
    int nbItem = d->scene->items().size();
    if(nbItem > 5)
        return;

    d->scene->addItem(pixmap);
    if(!image.isNull())
        pixmap->setPixmap(QPixmap::fromImage(image));
    else
        pixmap->setPixmap(QPixmap(":/medGui/pixmaps/default_thumbnail.png"));

    int width(medDatabaseThumbnailHelper::width), height(medDatabaseThumbnailHelper::height);

    QPen pen(QColor(70,70,70));
    pen.setWidth(4);
    switch(nbItem)
    {
        case 0:
            break;
        case 1:
            d->scene->addLine(width + pen.width()/2,
                    0,
                    width + pen.width()/2,
                    (height  + pen.width()/2) * 2,
                    pen
                    );
            d->scene->addLine(0,
                    height + pen.width()/2,
                    (width + pen.width()/2) * 2,
                    height + pen.width()/2,
                    pen
                    );
            pixmap->moveBy(width + pen.width() + 1,
                    0
                    );
            break;
        case 4:
            pixmap->moveBy(0,
                    height + pen.width() +1
                    );
            break;
        case 5:
            pixmap->moveBy(width + pen.width() +1,
                    height + pen.width() + 1
                    );
            break;
        default:
            break;
    }

    this->fitInView(d->scene->sceneRect(), Qt::KeepAspectRatio);
}

QLabel* medDatabasePreview::label()
{
    return d->label;
}


