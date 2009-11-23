#include "medDatabasePreviewItem.h"
#include "medDatabasePreviewItemLoader.h"

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItemPrivate
{
public:
    QString path;
};

medDatabasePreviewItem::medDatabasePreviewItem(const QString& path, QGraphicsItem *parent) : QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabasePreviewItemPrivate)
{
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
    return new medDatabasePreviewItem(d->path);
}

QString medDatabasePreviewItem::path(void) const
{
    return d->path;
}

void medDatabasePreviewItem::setImage(const QImage& image)
{
    this->setPixmap(QPixmap::fromImage(image));
}
