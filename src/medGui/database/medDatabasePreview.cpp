#include "medDatabasePreview.h"

#include "medDataIndex.h"
#include "medAbstractDbController.h"
#include "medDataManager.h"
#include "medImageFileLoader.h"


#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>

class medDatabasePreviewPrivate
{
public:
    QGraphicsPixmapItem *pixmap;
    QGraphicsScene *scene;
    QLabel *label;
};


medDatabasePreview::medDatabasePreview(QWidget *parent): d(new medDatabasePreviewPrivate)
{
    d->scene = new QGraphicsScene;
    this->setScene(d->scene);

    d->pixmap = new QGraphicsPixmapItem;
    d->pixmap->setPixmap(QPixmap(":/medGui/pixmaps/default_thumbnail2.png"));
    this->fitInView(d->pixmap, Qt::KeepAspectRatio);
    d->scene->addItem(d->pixmap);

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
    this->fitInView(d->pixmap, Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(event);
}

void medDatabasePreview::update(const medDataIndex &index)
{

    medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
    QString thumbpath = dbc->metaData(index, medMetaDataKeys::ThumbnailPath);

    bool shouldSkipLoading = false;
    if ( thumbpath.isEmpty() )
    {
        // first try to get it from controller
        QImage thumbImage = dbc->thumbnail(index);
        if (!thumbImage.isNull())
        {
            this->setImage( thumbImage );
            shouldSkipLoading = true;
        }
    }

    if (!shouldSkipLoading)
    {
        medImageFileLoader *loader = new medImageFileLoader(thumbpath);
        connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));
        QThreadPool::globalInstance()->start(loader);
    }

    QString itemDescription = dbc->metaData(index, medMetaDataKeys::SeriesDescription);
    if (itemDescription.isEmpty())
        itemDescription = dbc->metaData(index, medMetaDataKeys::StudyDescription);
    if (itemDescription.isEmpty())
        itemDescription = dbc->metaData(index, medMetaDataKeys::PatientName);

    d->label->setText(itemDescription);
}

void medDatabasePreview::setImage(const QImage &image)
{
    d->pixmap->setPixmap(QPixmap::fromImage(image));
    this->fitInView(d->pixmap, Qt::KeepAspectRatio);
}

QLabel* medDatabasePreview::label()
{
    return d->label;
}
