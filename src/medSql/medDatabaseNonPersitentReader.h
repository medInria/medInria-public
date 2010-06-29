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

#ifndef MEDDATABASEREADER_H
#define MEDDATABASEREADER_H

#include <QtCore/QObject>

#include <medCore/medDataIndex.h>

class dtkAbstractData;

class medDatabaseNonPersitentReaderPrivate;

class medDatabaseNonPersitentReader : public QObject
{
    Q_OBJECT

public:
     medDatabaseNonPersitentReader(const QString& file);
    ~medDatabaseNonPersitentReader(void);

    medDataIndex run(void);

signals:
    void success(QObject *);
    void failure(QObject *);
    void progressed(int);

private:
    medDatabaseNonPersitentReaderPrivate *d;
};

#endif
