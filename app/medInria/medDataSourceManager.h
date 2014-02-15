/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

#include "medCoreExport.h"



class medAbstractDataSource;
class medDataSourceManagerPrivate;
class medDataIndex;
class medAbstractData;
class medDatabaseDataSource;


class MEDCORE_EXPORT medDataSourceManager : public QObject
{
    Q_OBJECT

public:
    /**
* instance - singleton access method, returns a singleViewContainer static instance of the manager
* @return medSettingsManager * - the manager
*/
    static medDataSourceManager *instance();

    /**
* destroy - should be called on closing the application, to destroy the singleton
*/
    static void destroy();


    QList<medAbstractDataSource*> dataSources();
    medDatabaseDataSource* databaseDataSource();


protected slots:

    /**
* @brief Export data from a selected medDataIndex in a data source.
*
* Opens a file dialog and tries writers based
* on the file extension given by the user.
* @param index
*/
    void exportData(const medDataIndex &index);
    void importData(medAbstractData *data);
    void importFile(QString path);
    void indexFile(QString path);
    void emitDataReceivingFailed(QString fileName);


signals:
    void open(const medDataIndex&);
    void open(QString);
    void load(QString);

protected:
    void connectDataSource(medAbstractDataSource* dataSource);


    medDataSourceManager();
    ~medDataSourceManager();


private:

    static medDataSourceManager *s_instance;
    medDataSourceManagerPrivate *d;
};
