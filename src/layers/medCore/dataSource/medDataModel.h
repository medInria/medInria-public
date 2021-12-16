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
#include <medDataModelElement.h>

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
    medDataModel(QObject *parent = nullptr);
	~medDataModel();

    bool setDefaultWorkingSource(unsigned int i);
    bool getSourceGlobalInfo(QString const &pi_sourceIntanceId, bool &pi_bOnline, bool &pi_bWritable, bool & pi_bLocal, bool &pi_bCache);
    bool getLevelMetaData(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QString const & key, QVariantList & po_entries);
    bool getLevelAttributes(QString const & pi_sourceIntanceId, unsigned int pi_uiLevel, QStringList & po_attributes);
    bool getLevelCount(QString const & pi_sourceIntanceId, unsigned int &po_uiLevelMax);


    QString getInstanceName(QString const & pi_sourceIntanceId);
    QList<medSourceItemModel*> models(); // rediscuté de son nom

    medSourceItemModel* getModel(QString const & pi_sourceIntanceId);

    medAbstractData * getData(medDataIndex const & index);

public slots:
   void addSource(medAbstractSource* pi_source);
   void removeSource(medAbstractSource* pi_source);

   void addData(medAbstractData * pi_dataset, QString uri);   //uri -> sourceInstanceId/IdLevel1/IdLevel.../IdLevelN
   void addData(medDataIndex * pi_datasetIndex, QString uri); //uri -> sourceInstanceId/IdLevel1/IdLevel.../IdLevelN


   void refresh(QString uri);   //uri -> sourceInstanceId/IdLevel1/IdLevel.../IdLevelN
   void sourceIsOnline(QString sourceIntanceId);

private:


signals:
    void sourceAdded(medAbstractSource*);   // Signal to indicate a source was registered
	void sourceRemoved(QString); // Signal to indicate a source was unregistered
    //void sourceOnline(QString, bool);

private:
    QMap< QString, medAbstractSource*> m_sourceIdToInstanceMap;
    QMap< medAbstractSource*, medSourceItemModel*> m_sourcesModelMap;
    medAbstractSource* m_defaultSource;

    QMap<medDataIndex, dtkSmartPointer<medAbstractData> > m_IndexToData;
};