#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
    medDatabaseDataSource(QWidget *parent = nullptr);
    ~medDatabaseDataSource();

    QString  tabName();
    QWidget* buildSourcesBrowser();
    QWidget* buildSourceListSelectorWidget();

    QWidget* buildSourcesTreeViewList();

    QList<medToolBox*> getToolBoxes();

public slots:
    void currentSourceChanged(int current);

signals:
    void openOnDoubleClick(medDataIndex);
    void changeSource(int current);

protected slots:
    //void onFilter(const QString &text, int column);
    //void compactFilter(const QString &text, int column);

private:
    medDatabaseDataSourcePrivate* d;

    // Hérité via medAbstractDataSource
    virtual QWidget * mainViewWidget() override;
    virtual QWidget * sourceSelectorWidget() override;
    virtual QString description() const override;
};
