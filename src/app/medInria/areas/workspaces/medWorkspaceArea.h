/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtGui>
#include <QtWidgets>

#include <medAbstractProcessLegacy.h>
#include <medAbstractWorkspaceLegacy.h>

class medAbstractView;

class medDataIndex;
class medToolBox;
class medViewContainer;
class medTabbedViewContainers;
class medAbstractDataSource;
class medWorkspaceAreaPrivate;
class medDatabaseDataSource;

/**
* @brief Handles all connections between all elements of the Viewer area.
* In this Area or workspace all visualization will take place.
* The object is instantiated by the medMainWindow to display the viewer workspaces.
*
*/
class medWorkspaceArea : public QWidget
{
    Q_OBJECT

public:
     /**
* @brief
*
* @param parent
*/
     medWorkspaceArea(QWidget *parent = nullptr);
    /**
* @brief
*
* @param void
*/
    ~medWorkspaceArea();

    QPixmap grabScreenshot();
    void grabVideo();

    bool setupWorkspace(const QString& id);

    bool setCurrentWorkspace(medAbstractWorkspaceLegacy* workspace);
    bool setCurrentWorkspace(const QString& id);

    medAbstractWorkspaceLegacy* currentWorkspace();

protected:
    void addDatabaseView(medDatabaseDataSource* dataSource);
    void switchToStackedViewContainers(medTabbedViewContainers* stack);

    /**
     * @brief getExportVideoDialogParameters open a window displaying video export parameters and send results
     */
    QVector<int> getExportVideoDialogParameters(int numberOfFrames, int numberOfSlices);

    /**
     * @brief runExportVideoProcess send each frame of the video to process
     * @param process for video export
     * @param current screenshot number
     */
    void runExportVideoProcess(medAbstractProcessLegacy *process, int screenshotCount);

signals:
    void open(const medDataIndex&);

private:
    friend class medMessageController;

private:
    medWorkspaceAreaPrivate *d;
};

