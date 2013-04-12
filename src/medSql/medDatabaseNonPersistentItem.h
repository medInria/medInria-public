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

#pragma once

#include "medSqlExport.h"

#include <medDataIndex.h>

#include <QtCore/QObject>
#include <QtCore/QList>

class dtkAbstractData;

class medDatabaseNonPersistentItemPrivate;

class MEDSQL_EXPORT medDatabaseNonPersistentItem : public QObject
{
    Q_OBJECT

public:
     medDatabaseNonPersistentItem();
    ~medDatabaseNonPersistentItem();

    const QString& name() const;
    const QString& birthdate() const;
    const QString& patientId() const;
    const QString& studyName() const;
    const QString& seriesName() const;
    const QString& file() const;
    const QString& studyId() const;
    const QString& studyUid() const;
    const QString& seriesId() const;
    const QString& seriesUid() const;
    const QImage& thumb() const;

    const medDataIndex& index() const;
    
    dtkAbstractData *data();

private:
    friend class medDatabaseNonPersitentController;
    friend class medDatabaseNonPersistentReader;
    friend class medDatabaseNonPersistentImporter;

private:
    medDatabaseNonPersistentItemPrivate *d;
};


