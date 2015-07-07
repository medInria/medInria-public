/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkAbstractData.h>

#include <medCoreExport.h>

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
    medAbstractData( medAbstractData * parent = NULL );
    virtual ~medAbstractData();

    void setDataIndex(const medDataIndex& index);
    medDataIndex dataIndex() const;

    QList< medAttachedData * > attachedData() const;

    medAbstractData *convert(const QString &type);

    virtual QImage& thumbnail();

public slots:

    void clearAttachedData();
    void addAttachedData( medAttachedData * data );
    void removeAttachedData( medAttachedData * data );
    void invokeModified();
    void generateThumbnail();

signals:

    //! emitted when an attached data is added
    void attachedDataAdded( medAttachedData * );

    //! emitted when an attached data is removed
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

Q_DECLARE_METATYPE(medAbstractData)
Q_DECLARE_METATYPE(medAbstractData *)

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
