/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDataSource.h>
#include <medDataIndex.h>

class medDatabaseDataSourcePrivate;

/**
* Not a classical data source per se, as it does not import data
* Still it fits to the design of data sources
*/
class medDatabaseDataSource : public medAbstractDataSource
{
    Q_OBJECT

public:
    medDatabaseDataSource(QWidget *parent = 0);
    ~medDatabaseDataSource();

    static bool registered();

    QWidget* mainViewWidget();
    QWidget* compactViewWidget();
    virtual QWidget* dialogWidget();

    QWidget* sourceSelectorWidget();

    QString tabName();

    QList<medToolBox*> getToolBoxes();

QString description() const;

public slots:
    void onOpeningFailed(const medDataIndex& index, QUuid);

signals:
    void open(const medDataIndex&);

protected slots:
    void onFilter(const QString &text, int column);


private:
    medDatabaseDataSourcePrivate* d;

};

medAbstractDataSource *createmedDatabaseDataSource ( QWidget* );
