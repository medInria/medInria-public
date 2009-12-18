/* medDatabasePreviewItemLoader.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:43:31 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:43:31 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEPREVIEWITEMLOADER_H
#define MEDDATABASEPREVIEWITEMLOADER_H

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItemLoaderPrivate;

class medDatabasePreviewItemLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
     medDatabasePreviewItemLoader(const QString& path);
    ~medDatabasePreviewItemLoader(void);

    void run(void);

signals:
    void completed(const QImage& image);

private:
    medDatabasePreviewItemLoaderPrivate *d;
};

#endif // MEDDATABASEPREVIEWITEMLOADER_H
