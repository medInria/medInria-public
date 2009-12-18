/* medDatabaseNavigatorItemLoader.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:40:22 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:40:23 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

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
