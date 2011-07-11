/* medDatabaseNavigatorItemLoader.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:40:14 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:40:15 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENAVIGATORITEMLOADER_H
#define MEDDATABASENAVIGATORITEMLOADER_H

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorItemLoaderPrivate;

class medDatabaseNavigatorItemLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
     medDatabaseNavigatorItemLoader(const QString& path);
    ~medDatabaseNavigatorItemLoader(void);

    void run(void);

signals:
    void completed(const QImage& image);

private:
    medDatabaseNavigatorItemLoaderPrivate *d;
};

#endif // MEDDATABASENAVIGATORITEMLOADER_H
