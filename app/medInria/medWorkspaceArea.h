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

#include <medWorkspace.h>

class dtkAbstractView;

class medDataIndex;
class medToolBox;
class medViewContainer;
class medTabbedViewContainers;
class medAbstractDataSource;
class medWorkspaceAreaPrivate;

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
     medWorkspaceArea(QWidget *parent = 0);
    /**
     * @brief
     *
     * @param void
    */
    ~medWorkspaceArea();

    
    QPixmap grabScreenshot();
    

public slots:
    /**
     * @brief Sets up the newly chosen workspace.
     *
     * When a workspace is switched to, sets up all the signal/slot connections
     * to raise the new medWorkspace.
     * This calls the factory if the workspace is not yet in the private
     * hash table of workspaces
     *
     * @param name the identifyer/description of the workspace.
    */
    void setupWorkspace(QString name);

    /**
     * @brief Split the currently displayed custom container.
     *
     *  This slot makes the connection between the layout toolbox gui and
     *  the actual custom view container.
     *
     * @param rows
     * @param cols
    */
    void split(int rows, int cols);

    /**
     * @brief Open data corresponding to index \param index.
     *
     * @param index Could describe just an image, or a complete Series,
     * Study or even Patient.
    */
    bool open(const medDataIndex& index);

    /**
     * @brief Open data corresponding to index \param index in a new tab.
     *
     * @param index Could describe just an image, or a complete Series,
     * Study or even Patient.
     */
    bool openInTab(const medDataIndex& index);

    /**
     * @brief Opens file on the local filesystem.
     *
     * Calls open(const medDataIndex& index) with a
     * new index created from the medDatabaseNonPersistentController
     * @param file
    */
    void open(const QString& file);

    /**
     * @brief Opens file on the local filesystem.
     *
     * Calls openInTab(const medDataIndex& index) with a
     * new index created from the medDatabaseNonPersistentController
     * @param file
    */
    void openInTab(const QString& file);

    /**
     * @brief  Update the medToolBoxes and frees the memory.
     *
     * Removes the view from the medViewManager
     * @param void
    */
    void onViewClosed();
    void onViewClosed(dtkAbstractView *);

    /**
     * @brief  Updates the medToolBoxes when a layer (data) has been removed from a view.
     *
     * @param void
    */
    void onDataRemoved(int layer);


    /**
     * @brief Switches between stack of containers
     * (from one workspace to an other)
     *
     * Called from the setupWorkspace method.
     * @param stack
    */
    void switchToStackedViewContainers(medTabbedViewContainers* stack);

    /**
     * @brief Switches between medViewContainers.
     *
     * The list of containers is different from one workspace to an other.
     * This method tries to switch to a container of name \param name.
     * If it doesn't exist, does nothing.
     *
     * @param name the identifyer/description of the container.
    */
    void switchToContainer(const QString& name);

    /**
     * @brief Set custom view preset
     *  Presets are defined in medCustomViewContainer::Preset.
     *  @todo change from int to Preset
     * @param index
    */
    void switchToContainerPreset(int index);

    /**
     * @brief Adds a medToolBox to the medToolBoxContainer.
     *
     * @param toolbox
    */
    void addToolBox(medToolBox *toolbox);

    /**
     * @brief Removes a medToolBox from the medToolBoxContainer.
     *
     * @param toolbox
    */
    void removeToolBox(medToolBox *toolbox);

    /**
     * @brief Switches from a medWorkspace::LayoutType to an other.
     *
     * This can't be done by the workspace which has no access to the
     * medWorkspaceArea Widget and its internal organisation.
     *
     * @param layout
    */
    void switchToLayout (medWorkspace::LayoutType layout);


    void onFileOpenedInTab(const medDataIndex &index);
    
    void addDataSource(medAbstractDataSource* dataSource);
    

protected slots:
    /**
     * @brief View focused callback.
     *
     *  This method updates the toolboxes according to the focused view.
     *
     * @param view
    */
    void onViewFocused(dtkAbstractView *view);

protected:
    /**
     * @brief Returns the current root view container.
     *
     * Note that containers are hierarchical structures.  This methods
     * returns the root container in the current
     * medTabbedViewContainers.
     *
     * @param void
     * @return medViewContainer *
    */
    medViewContainer *currentRootContainer();

    /**
     * @brief Returns the currently focused child container.
     *
     * Note that containers are hierarchical structures.
     * This methods returns a container that can contain a view.
     * It's a leaf in the container tree.
     *
     * @param void
     * @return medViewContainer *
    */
    medViewContainer *currentContainerFocused();
    
protected slots:
    /**
     * @brief Brings up or deletes a medClutEditor.
     *
     * @param checked
    */
    void bringUpTransferFunction(bool checked);

    /**
     * @brief Updates the transfer function of the current view
     *
    */
    void updateTransferFunction();

signals:
    /**
     * @brief Emitted when the patient has changed
     *
     * Allows to clear all patient related data in a medWorkspace.
     * Only emitted if the setting "system/clearOnPatientChange" is set to true.
     *
    */
    void clearOnPatientChange();

     /**
     * @brief Emitted to specify the slice to display
     *
     *  @param slice - the slice number
     *
    */
    void sliceSelected ( int slice );
    
    
    

private:
    friend class medMessageController;

private:
    medWorkspaceAreaPrivate *d;
};


