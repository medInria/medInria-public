/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractData.h>

#include <medCoreExport.h>

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
                setting non-used dtkAbstractData methods private
    *=========================================================================*/
private:

    virtual void draw(void) {}

    virtual void update(void);

    virtual void *output(void);
    virtual void *output(int channel);

    virtual double parameter(int channel);

    virtual void setParameter(int parameter);
    virtual void setParameter(int parameter, int channel);

    virtual void setParameter(qlonglong parameter);
    virtual void setParameter(qlonglong parameter, int channel);

    virtual void setParameter(float parameter);
    virtual void setParameter(float parameter, int channel);

    virtual void setParameter(double parameter);
    virtual void setParameter(double parameter, int channel);

    virtual void setParameter(const QString& parameter);
    virtual void setParameter(const QString& parameter, int channel);

    virtual void setParameter(dtkAbstractData *parameter);
    virtual void setParameter(dtkAbstractData *parameter, int channel);

    virtual void setParameter(dtkVectorReal parameter);
    virtual void setParameter(dtkVectorReal parameter, int channel);

    virtual QVariant toVariant(dtkAbstractData *data);
    virtual dtkAbstractData *fromVariant(const QVariant& v);

private:
    medAbstractDataPrivate* d;

};

#define MED_DATA_INTERFACE_NO_MOC(_name,_desc) \
public:\
    static QString staticName() {return QString::fromUtf8(_name);}\
    static QString staticDescription() {return QString::fromUtf8(_desc);}\
    virtual QString identifier() const {return staticIdentifier();}\
    virtual QString name() const {return staticName();}\
    virtual QString description() const {return staticDescription();}

#define MED_DATA_INTERFACE(_name,_desc) \
public:\
    static QString staticIdentifier() {return QString(staticMetaObject.className());}\
    MED_DATA_INTERFACE_NO_MOC(_name,_desc)
