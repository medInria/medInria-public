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
#include <medDefaultWritingPolicy.h>

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
    QList<medAbstractParameter *> filteringParameters(QString const & pi_sourceInstanceId);

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
    QUuid saveData(medAbstractData &data);

    void extractBasePath(medAbstractData * pi_pData, QStringList &pi_basePath);
    medAbstractSource * getDefaultWorkingSource();
    medAbstractSource * getSourceToWrite(QString pi_sourceIdDst = "");
    
    static QString uriAsString(QStringList pi_uri);
    static QStringList uriAsList(QString pi_uri);

    QString convertToPath(QStringList pi_Uri);
    bool createPath(QString pi_sourceId, QStringList pi_folders, QStringList &po_uri, QMap<int, QString> pi_knownKeys = QMap<int, QString>());
    //bool saveData(QStringList pi_Uri, QString pi_dataName, medAbstractData *pi_data);


    bool saveData3(medAbstractData *pi_pData, QString const &pi_baseName, QStringList pi_uri);
    bool writeResults(QString pi_sourceId, medAbstractData * pi_pData, QStringList pi_UriOfRelatedData, QString pi_basePath, medWritingPolicyData & pi_writingPolicyData, medAbstractWritingPolicy * pi_pWritingPolicy);

    /** *****************************************************************************************************************/
    /** *****************************************************************************************************************/
    /**  TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO **/
    /** *****************************************************************************************************************/
    /** *****************************************************************************************************************/
    /**                                                                                                                **/
    /** Implement a mechanism substitute temporary id of data to final id given by source                              **/
    /** to do this:                                                                                                    **/
    /**   - keep a list of temporaries id of data                                                                      **/
    /**   - keep a list request id and type of request associated to it (data, key, path)                              **/
    /**   - keep a mapping between them                                                                                **/
    /**   - listen the signal finish of source                                                                         **/
    /**   - call the getAsyncResults on a source                                                                       **/
    /**                                                                                                                **/
    /** *****************************************************************************************************************/
    /** *****************************************************************************************************************/


    void expandAll(const QString &sourceInstanceId);

    medAbstractWritingPolicy* getSourceWPolicy(QString pi_sourceId);
    medAbstractWritingPolicy* getGeneralWPolicy(){ return &m_generalWritingPolicy; }
    //medAbstractWritingPolicy* getWPolicy(medAbstractWritingPolicy * pi_policy, QString pi_sourceId = "");

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
    QStringList m_sourceInstanceIdOrderedList;
    QMap< QString, medAbstractSource*> m_sourceIdToInstanceMap;
    QMap< medAbstractSource*, medSourceItemModel*> m_sourcesModelMap; //TODO delete medSourceItemModel* in destructor
    medAbstractSource* m_defaultSource;

    QMap<medDataIndex, dtkSmartPointer<medAbstractData> > m_IndexToData;
    medDefaultWritingPolicy m_generalWritingPolicy;
    static medDataModel * s_instance;
};