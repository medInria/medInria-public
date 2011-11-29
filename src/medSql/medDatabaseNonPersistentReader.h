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

#include "medSqlExport.h"

#include <QtCore/QObject>

#include <medDataIndex.h>
#include <medJobItem.h>

class dtkAbstractData;

class medDatabaseNonPersistentReaderPrivate;

/**
* @brief Reads files and adds them to the medNonPersistentDatabase.
*
* This class inherits from medJobItem and is meant to be run by the medJobManager.
*
*/
class MEDSQL_EXPORT medDatabaseNonPersistentReader : public medJobItem
{
    Q_OBJECT

public:
    /**
    * @brief Constructor.
    *
    * @param file the file or directory to be read.
    * @param callerUuid The string representation of a unique identifier. The caller will react to link the final signal with this id to know whether it should react to it or not.
    */
    medDatabaseNonPersistentReader(const QString& file,const QString& callerUuid = QString());
    ~medDatabaseNonPersistentReader(void);

    void run(void);

    /**
      Returns the index of the data which has been read. Index is not
      valid if reading was not successful.
    */
    medDataIndex index(void) const;

signals:

    /**
    * @brief Signal emitted when the reading is complete (or has failed).
    *
    * @param index the new medDatataIndex, or an invalid one
    * @param callerUuid the identifier from the caller.
    */
    void nonPersistentRead(const medDataIndex & index,const QString& callerUuid);

public slots:
    void onCancel(QObject*);

private:
    medDatabaseNonPersistentReaderPrivate *d;
};

#endif
