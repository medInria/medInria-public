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

#include<QMap>

class medDataModel : public QObject
{

    Q_OBJECT

public:	
    medDataModel(QObject *parent = nullptr);
	~medDataModel();

    bool setDefaultWorkingSource(unsigned int i);
    bool getSourceGlobalInfo(QString const &pi_sourceIntanceId, QStringList &pi_columnName, bool &pi_bOnline, bool &pi_bWritable, bool &pi_bCache);
    bool getInfoLevel(QString const &pi_sourceIntanceId, unsigned int pi_uiLevel, QVariantList &po_entries);

public slots:
   void addSource(medAbstractSource* pi_source);
   void removeSource(medAbstractSource* pi_source);

   void addData(medAbstractData * pi_dataset, QString uri); //uri -> sourceInstanceId/IdLevel1/IdLevel.../IdLevelN
   void addData(medDataIndex * pi_datasetIndex, QString uri); //uri -> sourceInstanceId/IdLevel1/IdLevel.../IdLevelN
private:


signals:
    void sourceAdded(medAbstractSource*);  // Signal to indicate a source was registered
	void sourceRemoved(medAbstractSource*); // Signal to indicate a source was unregistered

private:
    QMap< medAbstractSource*,  medDataModelElement*> m_sourcesModelMap;
    medAbstractSource* m_defaultSource;
};