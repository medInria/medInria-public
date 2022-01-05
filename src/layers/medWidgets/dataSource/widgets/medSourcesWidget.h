#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/


//#include <medSourceItemModelPresenter.h>
#include <medDataModel.h>

#include <QWidget>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

#include <medWidgetsExport.h>

class medSourceItemModelPresenter;

class medSourcesWidget : public QWidget
{
    Q_OBJECT
public:
    medSourcesWidget();
    ~medSourcesWidget();

    void addSources(medDataModel *dataModel);

public slots:
    void addSource(medDataModel *dataModel, QString sourceInstanceId);
    void removeSource(QString sourceInstanceId);
    void filter(const QString &);

private:
    QVBoxLayout m_layout;
    QMap<QString, QWidget*> m_treeMap; // sourceInstanceId, SubTreeWidget
    QMap<QString, QWidget*> m_titleMap;  // sourceInstanceId, source instance names
};
