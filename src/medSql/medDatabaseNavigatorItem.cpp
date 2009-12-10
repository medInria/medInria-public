#include "medDatabaseNavigatorItem.h"
#include "medDatabaseNavigatorItemLoader.h"

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorItemPrivate
{
public:
    int patientId;
    int studyId;
    int seriesId;
    int imageId;

    QString path;
};

medDatabaseNavigatorItem::medDatabaseNavigatorItem(int patientId, int studyId, int seriesId, int imageId, const QString &path, QGraphicsItem *parent) : QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabaseNavigatorItemPrivate)
{
    d->patientId = patientId;
    d->studyId = studyId;
    d->seriesId = seriesId;
    d->imageId = imageId;

    d->path = path;

    medDatabaseNavigatorItemLoader *loader = new medDatabaseNavigatorItemLoader(path);

    connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));

    QThreadPool::globalInstance()->start(loader);
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
    if(d->imageId >= 0)
        emit imageClicked(d->imageId);

    else if(d->seriesId >= 0)
        emit seriesClicked(d->seriesId);

    else if(d->studyId >= 0)
        emit studyClicked(d->studyId);

    else if(d->patientId >= 0)
        emit patientClicked(d->patientId);
}
