#include "medDatabasePreviewItem.h"
#include "medDatabasePreviewItemLoader.h"

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItemPrivate
{
public:
    int patientId;
    int studyId;
    int seriesId;
    int imageId;

    QString path;
};

medDatabasePreviewItem::medDatabasePreviewItem(int patientId, int studyId, int seriesId, int imageId, const QString& path, QGraphicsItem *parent) : QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabasePreviewItemPrivate)
{
    d->patientId = patientId;
    d->studyId = studyId;
    d->seriesId = seriesId;
    d->imageId = imageId;

    d->path = path;

    medDatabasePreviewItemLoader *loader = new medDatabasePreviewItemLoader(path);

    connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));

    QThreadPool::globalInstance()->start(loader);
}

medDatabasePreviewItem::~medDatabasePreviewItem(void)
{
    delete d;

    d = NULL;
}

medDatabasePreviewItem *medDatabasePreviewItem::clone(void)
{
    return new medDatabasePreviewItem(d->patientId, d->studyId, d->seriesId, d->imageId, d->path);
}

int medDatabasePreviewItem::patientId(void) const
{
    return d->patientId;
}

int medDatabasePreviewItem::studyId(void) const
{
    return d->studyId;
}

int medDatabasePreviewItem::seriesId(void) const
{
    return d->seriesId;
}

int medDatabasePreviewItem::imageId(void) const
{
    return d->imageId;
}

QString medDatabasePreviewItem::path(void) const
{
    return d->path;
}

void medDatabasePreviewItem::setImage(const QImage& image)
{
    this->setPixmap(QPixmap::fromImage(image));
}
