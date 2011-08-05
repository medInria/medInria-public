/* medDatabaseNonPersitentReader.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Jun 29 15:25:31 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 15:54:34 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 15
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENONPERSISTENTREADER_H
#define MEDDATABASENONPERSISTENTREADER_H

#include <QtCore/QObject>

#include <medDataIndex.h>
#include <medJobItem.h>

class dtkAbstractData;

class medDatabaseNonPersistentReaderPrivate;

class medDatabaseNonPersistentReader : public medJobItem
{
    Q_OBJECT

public:
     medDatabaseNonPersistentReader(const QString& file);
    ~medDatabaseNonPersistentReader(void);

    void run(void);

signals:
    void success(QObject *);
    void failure(QObject *);
    void progressed(int);
    void nonPersistentRead(const medDataIndex &);

public slots:
    void onCancel(QObject*);

private:
    medDatabaseNonPersistentReaderPrivate *d;
};

#endif
