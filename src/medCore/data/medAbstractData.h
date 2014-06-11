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

#include <medCoreExport.h>
#include <medDataEnum.h>

class medAbstractDataPrivate;

class medAttachedData;
class medDataIndex;

/**
 * Extending medAbstractData class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractData : public dtkAbstractData
{
    Q_OBJECT

public:
    medAbstractData( medAbstractData * parent = NULL );
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
     * @brief Get attached data (like histogram, annotations etc.)
     *
     * @return QList< medAttachedData * >
     */
    QList< medAttachedData * > attachedData() const;

    void setDataCategory(medData::Category type);
    medData::Category dataCategory() const;

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

    void generateThumbnail();

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


    /*=========================================================================
                 NOT IMPLEMENTED ON PURPOSE
    *=========================================================================*/
private:

    using dtkAbstractData::draw;
    using dtkAbstractData::update;
    using dtkAbstractData::output;
    using dtkAbstractData::parameter;
    using dtkAbstractData::setParameter;
    using dtkAbstractData::toVariant;
    using dtkAbstractData::fromVariant;

private:
    medAbstractDataPrivate* d;

};


