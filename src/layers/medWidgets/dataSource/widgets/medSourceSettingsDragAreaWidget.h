#pragma once
/*
 * medInria
 * Copyright (c) INRIA 2013. All rights reserved.
 * 
 * medInria is under BSD-2-Clause license. See LICENSE.txt for details in the root of the sources or:
 * https://github.com/medInria/medInria-public/blob/master/LICENSE.txt
 * 
 * This software is distributed WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include <QListWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <medSourcesLoader.h>
#include <medSourceSettingsWidget.h>
#include <medWidgetsExport.h>

class MEDWIDGETS_EXPORT medSourceSettingsDragAreaWidget : public QListWidget
{
    Q_OBJECT
    
public:
    explicit medSourceSettingsDragAreaWidget(medSourcesLoader * pSourceLoader, 
                                             QTreeWidget * sourceInformation, 
                                             QWidget *parent = nullptr);

    void addNewSourceItem(medAbstractSource * pSource);
    void setSelectedVisualisation(medAbstractSource * pSource);
    void setSourceToDefault(medAbstractSource * pSource);
    void setConnectionIcon(medAbstractSource * pSource, bool connection);
    void removeSourceItem(QString sourceName);
    void switchMinimization(medSourceSettingsWidget* sourceWidget, bool isMinimized);

    void rowMoved(const QModelIndex &parent, 
                  int start, 
                  int end, 
                  const QModelIndex &destination, 
                  int row);

signals:
    void sourceItemChosen(medAbstractSource *); // Signal to indicate this source is selected

private:
    medSourcesLoader * _pSourceLoader;
    QTreeWidget * _sourceInformation;
};
