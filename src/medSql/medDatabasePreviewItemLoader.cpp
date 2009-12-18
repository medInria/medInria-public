/* medDatabasePreviewItemLoader.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:43:36 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:43:36 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

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
