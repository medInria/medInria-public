#include "medDatabasePreviewItemLoader.h"

class medDatabasePreviewItemLoaderPrivate
{
public:
    QImage image;
    QString path;
};

medDatabasePreviewItemLoader::medDatabasePreviewItemLoader(const QString& path) : d(new medDatabasePreviewItemLoaderPrivate)
{
    d->path = path;
}

medDatabasePreviewItemLoader::~medDatabasePreviewItemLoader(void)
{
    delete d;

    d = NULL;
}

void medDatabasePreviewItemLoader::run(void)
{
    if(!d->path.isEmpty()) {
        QImageReader reader(d->path);
        d->image = reader.read();
    }

    emit completed(d->image);
}
