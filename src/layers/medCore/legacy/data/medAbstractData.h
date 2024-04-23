#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractData.h>

#include <medCoreExport.h>
#include <medMetaDataKeys.h>

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

    medAbstractData( medAbstractData *parent = nullptr );
    medAbstractData( const medAbstractData &other );
//    TODO Must be implemented as soon as possible
//    medAbstractData& operator= (medAbstractData const & data);
//    medAbstractData& operator= (medAbstractData const && data);
//    medAbstractData( const medAbstractData &&other );

    virtual ~medAbstractData();

    virtual medAbstractData* clone() override;

    void setDataIndex(const medDataIndex& index);
    medDataIndex dataIndex() const;

    QList< medAttachedData * > attachedData() const;

    medAbstractData *convert(const QString &type);

    virtual QImage generateThumbnail(QSize size);

    // FLO & JU
    bool addParentData(medAbstractData * pi_parentData);
    bool addDerivedData(medAbstractData * pi_derivedData);
    bool removeParentData(medAbstractData * pi_parentData);
    bool removeDerivedData(medAbstractData * pi_derivedData);
    QList<medAbstractData*> parentData();
    QList<medAbstractData*> derivedData();
    void setExpectedName(QString name);
    QString getExpectedName();


    // //void addMetaData(const QString& key, const QStringList& values);
    // //void addMetaData(const QString& key, const QString& value);
    // void setMetaData(const QString& key, const QStringList& values) { setMetaData(Key2(key), values); }
    // void setMetaData(const QString& key, const QString& value)      { setMetaData(Key2(key), value); }
    // 
    // QStringList metaDataList(void) const;
    // QStringList metaDataValues(const QString& key) const { return metadatas(key); }
    // QString metadata(const QString& key) const { return metadata(Key2(key)); }
    // QStringList metadatas(const QString& key) const { return metadatas(Key2(key)); }
    // 
    // 
    // QStringList metadatas(Key2 const &key);
    // QString     metadata(Key2 const &key) { auto resLst = metadatas(key); return resLst.size() == 0 ? QString() : resLst[0]; }
    // void        setMetaData(Key2 const &key, QStringList value);
    // void        setMetaData(Key2 const &key, QString value);
    QString fecthMetaData(QString word);

    QMap<QString, QString> getMetaDataMap() const;



    // //void addMetaData(const QString& key, const QStringList& values);
    // //void addMetaData(const QString& key, const QString& value);
    // void setMetaData(const QString& key, const QStringList& values) { setMetaData(Key2(key), values); }
    // void setMetaData(const QString& key, const QString& value)      { setMetaData(Key2(key), value); }
    // 
    // QStringList metaDataList(void) const;
    // QStringList metaDataValues(const QString& key) const { return metadatas(key); }
    // QString metadata(const QString& key) const { return metadata(Key2(key)); }
    // QStringList metadatas(const QString& key) const { return metadatas(Key2(key)); }
    // 
    // 
    // QStringList metadatas(Key2 const &key);
    // QString     metadata(Key2 const &key) { auto resLst = metadatas(key); return resLst.size() == 0 ? QString() : resLst[0]; }
    // void        setMetaData(Key2 const &key, QStringList value);
    // void        setMetaData(Key2 const &key, QString value);
    QString fecthMetaData(QString word) const;

    QMap<QString, QString> getMetaDataMap() const;

    //virtual QByteArray serialise() const = 0 ;
public slots:
    void clearAttachedData();
    void addAttachedData( medAttachedData * data );
    void removeAttachedData( medAttachedData * data );
    void invokeModified();

signals:

    //! emitted when an attached data is added
    void attachedDataAdded( medAttachedData * );

    //! emitted when an attached data is removed
    void attachedDataRemoved( medAttachedData * );

    //! Signal emitted when the data contents have been altered.
    void dataModified(medAbstractData *);

protected slots:

    virtual QImage generateThumbnailInGuiThread(QSize size);

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
    using dtkAbstractData::thumbnail;
    using dtkAbstractData::thumbnails;

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
