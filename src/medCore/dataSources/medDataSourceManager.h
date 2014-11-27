/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>
#include <QUuid>

#include <medDataIndex.h>

class medAbstractDataSource;
class medDataSourceManagerPrivate;
class medAbstractData;
class medDatabaseDataSource;


class medDataSourceManager : public QObject
{
    Q_OBJECT

public:
    static medDataSourceManager *instance();

    static void destroy();

    QList<medAbstractDataSource*> dataSources();


protected slots:
    void exportData(const medDataIndex &index);
    void importData(medAbstractData *data);
    void importFile(QString path);
    void indexFile(QString path);
    void emitDataReceivingFailed(QString fileName);

signals:
    void open(const medDataIndex&);
    void open(QString);
    void load(QString);
    void openFromPathRequest(QString path);
    void openFromIndexRequest(medDataIndex index);

protected:
    void connectDataSource(medAbstractDataSource* dataSource);


    medDataSourceManager();
    ~medDataSourceManager();


protected slots:
    void loadFromPath(QString path);

private:

    static medDataSourceManager *s_instance;
    medDataSourceManagerPrivate *d;
};
