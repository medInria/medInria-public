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

#include <medGuiExport.h>

class medAbstractView;
class medDataIndex;
class medViewContainer;
class medTabbedViewContainersPrivate;
class QUuid;

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


public slots:
    void addContainerInTab();
    void addContainerInTab(const QString &name);
    void insertContainerInTab(int index, const QString &name);
    void resetCurrentTab();
    void resetTab(int index);

private slots:
    void disconnectTabFromSplitter(int index);
    void repopulateCurrentTab();

signals:
    void newContainer(QUuid&);



private:
    medTabbedViewContainersPrivate *d;
};


