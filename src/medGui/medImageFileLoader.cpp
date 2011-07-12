/* medImageFileLoader.cpp ---
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

#include "medImageFileLoader.h"

#include <dtkCore/dtkLog.h>

class medImageFileLoaderPrivate
{
public:
    QImage image;
    QString path;
};

medImageFileLoader::medImageFileLoader(const QString& path) : d(new medImageFileLoaderPrivate)
{
    d->path = path;
}

medImageFileLoader::~medImageFileLoader(void)
{
    delete d;

    d = NULL;
}

void medImageFileLoader::run(void)
{
    if(!d->path.isEmpty()) {
        QImageReader reader(d->path);
        d->image = reader.read();
        if ( d->image.isNull() ) {
            dtkDebug() << "Failed to load image from " << d->path << " Reader error is " << reader.errorString();
        }
    }

    emit completed(d->image);
}
