/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractData.h>

#include "medCoreExport.h"

class medAbstractDataPrivate;

class medAttachedData;
class medDataIndex;

/**
 * Extending dtkAbstractData class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractData : public dtkAbstractData
{
    Q_OBJECT

public:
    medAbstractData( dtkAbstractData * parent = NULL );
    virtual ~medAbstractData();

    /**
    * Attach a meddataindex to the data to carry it arround
    * @params const medDataIndex & index
    */
    void setDataIndex(const medDataIndex& index);

    /**
    * Get the dataindex attached to the data or an invalid one
    * @return medDataIndex
    */
    medDataIndex dataIndex() const;

    /**
     * Set this to true if the volume has been generated with correct
     * values (e.g. for z-dimension)
     *
     * @params bool flag
     * @return void
     */
    void setTrueVolumetric(bool flag);

    /**
     * if set to true, the dataset is a valid 3d volume else operation
     * like MPR, VRT should not be possible
     *
     * @return bool
     */
    bool trueVolumetric() const;

    /**
     * @brief Get attached data (like histogram, annotations etc.)
     *
     * @return QList< medAttachedData * >
     */
    QList< medAttachedData * > attachedData() const;

    virtual QImage& thumbnail();

public slots:
    /**
     * @brief Clear the list of attached data
     *
     * @return void
     */
    void clearAttachedData();

    /**
     * @brief add attached data
     *
     * @return void
     */
    void addAttachedData( medAttachedData * data );

    /**
     * @brief remove attached data
     *
     * @return void
     */
    void removeAttachedData( medAttachedData * data );

    /** Invoke emit datamodified(this);
     * \sa dataModfied(medAbstractData *);
    */
    void invokeModified();

signals:
    /**
     * @brief emitted when an attached data is added
     */
    void attachedDataAdded( medAttachedData * );
    /**
     * @brief emitted when an attached data is removed
     */
    void attachedDataRemoved( medAttachedData * );

    //! Signal emitted when the data contents have been altered.
    void dataModified(medAbstractData *);

private:
    medAbstractDataPrivate* d;

};


