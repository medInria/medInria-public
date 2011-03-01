/* medDatabaseNonPersitentItem.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sun Jun 27 18:41:16 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Jun 29 16:09:12 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASENONPERSISTENTITEM_H
#define MEDDATABASENONPERSISTENTITEM_H

#include "medSqlExport.h"

#include <medCore/medDataIndex.h>

#include <QtCore/QObject>
#include <QtCore/QList>

class dtkAbstractData;

class medDatabaseNonPersistentItemPrivate;

class MEDSQL_EXPORT medDatabaseNonPersistentItem : public QObject
{
    Q_OBJECT

public:
     medDatabaseNonPersistentItem(void);
    ~medDatabaseNonPersistentItem(void);

    const QString& name(void) const;
    const QString& studyName(void) const;
    const QString& file(void) const;
    const QImage& thumb(void) const;

    const medDataIndex& index(void) const;
    
    dtkAbstractData *data(void);

private:
    friend class medDatabaseNonPersitentController;
    friend class medDatabaseNonPersistentReader;

private:
    medDatabaseNonPersistentItemPrivate *d;
};

#endif
