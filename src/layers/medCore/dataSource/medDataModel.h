#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medSourcesLoader.h>
#include <medSourceItemModel.h>

#include <medCoreExport.h>
#include <QMap>
#include <QList>
#include <QAbstractItemModelTester>

#include <medDataIndex.h>
#include <medAbstractData.h>

#include <dtkCoreSupport/dtkSmartPointer.h>

class MEDCORE_EXPORT medDataModel : public QObject
{

    Q_OBJECT
public:

    using datasetAttributes = QMap<QString, QString>;
    using levelAttributes = QList<datasetAttributes>;

    static medDataModel* instance(QObject *parent = nullptr);
	~medDataModel();

    bool setDefaultWorkingSource(unsigned int i);

    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Members functions to interrogate the source
    bool sourceGlobalInfo(QString const &pi_sourceIntanceId, bool &pi_bOnline, bool &pi_bWritable, bool & pi_bLocal, bool &pi_bCache);
    bool mandatoryAttributesKeys(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QStringList & po_attributes);
    bool attributesForBuildTree(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & key, levelAttributes & po_entries);
    bool mandatoriesAttributes(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & parentKey, levelAttributes & po_entries);
    bool optionalAttributes(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & key, datasetAttributes & po_attributes, datasetAttributes & po_tags);
    bool levelCount(QString const & pi_sourceIntanceId, unsigned int &po_uiLevelMax);
    
    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Members functions to access sourceItemModel
    void getModelData(QModelIndex &index, datasetAttributes &attributes, int role = 0);
    void setModelData(QModelIndex &index, datasetAttributes &attributes, int role = 0);
    void getModelMetaData(QModelIndex &index, datasetAttributes &attributes, datasetAttributes &tag);
    void setModelMetaData(QModelIndex &index, datasetAttributes &attributes, datasetAttributes &tag);



    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Members functions to deal with datamodel
    QString getInstanceName(QString const & pi_sourceIntanceId);
    QList<medSourceItemModel*> models(); // rediscute de son nom
    medSourceItemModel* getModel(QString const & pi_sourceIntanceId);

    // ////////////////////////////////////////////////////////////////////////////////////////////
    // Members functions to get Data, metadata and informations
    medAbstractData * getData(medDataIndex const & index);
    datasetAttributes getMetaData(QModelIndex const & index); //TODO Rename

    void expandAll(const QString &sourceInstanceId);

public slots:
   void addSource(medAbstractSource* pi_source);
   void removeSource(medAbstractSource* pi_source);

   void addData(medAbstractData * pi_dataset, QString uri);   //uri -> sourceInstanceId/IdLevel1/IdLevel.../IdLevelN
   void addData(medDataIndex * pi_datasetIndex, QString uri); //uri -> sourceInstanceId/IdLevel1/IdLevel.../IdLevelN


   void refresh(QString uri);   //uri -> sourceInstanceId/IdLevel1/IdLevel.../IdLevelN
   void sourceIsOnline(QString sourceIntanceId);

   void removeConvertedData(QObject *obj);

private:
    medDataModel(QObject *parent = nullptr);


signals:
    void sourceAdded(medAbstractSource*);   // Signal to indicate a source was registered
	void sourceRemoved(QString); // Signal to indicate a source was unregistered
    //void sourceOnline(QString, bool);

private:
    QMap< QString, medAbstractSource*> m_sourceIdToInstanceMap;
    QMap< medAbstractSource*, medSourceItemModel*> m_sourcesModelMap; //TODO delete medSourceItemModel* in destructor
    medAbstractSource* m_defaultSource;

    QMap<medDataIndex, dtkSmartPointer<medAbstractData> > m_IndexToData;

    static medDataModel * s_instance;
};