/* medDatabaseWriter.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jun 29 15:25:31 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 15:32:21 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 12
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEWRITER_H
#define MEDDATABASEWRITER_H

#include <QtCore/QObject>

#include <medCore/medJobItem.h>

#include <medCore/medDataIndex.h>

class dtkAbstractData;
class medDatabaseWriterPrivate;

class medDatabaseWriter : public QObject
{
    Q_OBJECT

public:
     medDatabaseWriter(dtkAbstractData *data);
    ~medDatabaseWriter(void);

    medDataIndex run(void);

signals:
    void success(QObject *);
    void failure(QObject *);
    void progressed(int);

private:
    medDatabaseWriterPrivate *d;
};

#endif
