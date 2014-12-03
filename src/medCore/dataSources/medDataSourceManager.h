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

signals:
    void openRequest(const medDataIndex&);

protected:
    void connectDataSource(medAbstractDataSource* dataSource);

protected:
    medDataSourceManager();
    ~medDataSourceManager();

private:

    static medDataSourceManager *s_instance;
    medDataSourceManagerPrivate *d;
};
