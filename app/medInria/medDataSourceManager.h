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

class MEDCORE_EXPORT medDataSourceManager : public QObject
{
    Q_OBJECT
    
public:
    /**
    * instance - singleton access method, returns a singleViewContainer static instance of the manager
    * @return   medSettingsManager * - the manager
    */
    static medDataSourceManager *instance();
    /**
    * destroy - should be called on closing the application, to destroy the singleton
    */
    static void destroy();
    
    void ceateDataSource();
    
signals:
    void registered(medAbstractDataSource* dataSources);
    
    void open(const medDataIndex&);
    void open(QString);
    void load(QString);
    
protected:
    medDataSourceManager();
    ~medDataSourceManager();
    

private:

    static medDataSourceManager *s_instance;

    medDataSourceManagerPrivate *d;
};

