#include "medDatabaseNavigatorItemLoader.h"

class medDatabaseNavigatorItemLoaderPrivate
{
public:
    QImage image;
    QString path;
};

medDatabaseNavigatorItemLoader::medDatabaseNavigatorItemLoader(const QString& path) : d(new medDatabaseNavigatorItemLoaderPrivate)
{
    d->path = path;
}

medDatabaseNavigatorItemLoader::~medDatabaseNavigatorItemLoader(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigatorItemLoader::run(void)
{
    if(!d->path.isEmpty()) {
        QImageReader reader(d->path);
        d->image = reader.read();
    }

    emit completed(d->image);
}
