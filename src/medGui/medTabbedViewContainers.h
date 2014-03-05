/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>
#include <QUuid>

#include <medGuiExport.h>

class medAbstractView;
class medDataIndex;
class medViewContainer;
class medTabbedViewContainersPrivate;

/**
 * @brief A QStackedWidget that contains medViewContainers.
 * There is one such stack per medViewWorkspace. 
 *
*/
class MEDGUI_EXPORT medTabbedViewContainers : public QTabWidget
{
    Q_OBJECT

public:
     medTabbedViewContainers(QWidget *parent = 0);
    ~medTabbedViewContainers();

    void lockTabs();
    void unlockTabs();
    void hideTabBar();
    QList<QUuid> containersSelected();


public slots:
    medViewContainer* addContainerInTab();
    medViewContainer* addContainerInTab(const QString &name);
    medViewContainer* insertContainerInTab(int index, const QString &name);
    void resetCurrentTab();
    void resetTab(int index);

private slots:
    void disconnectTabFromSplitter(int index);
    void repopulateCurrentTab();
    void addContainerToSelection(QUuid container);
    void removeContainerFromSelection(QUuid container);
    void connectContainer(QUuid container);
    void link(QUuid uuid, QString);
    void unlink(QUuid uuid);
    void buildTemporaryPool();

signals:
    void containersSelectedChanged();
    void currentLayerChanged();



private:
    medTabbedViewContainersPrivate *d;
};


