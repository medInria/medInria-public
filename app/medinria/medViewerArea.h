/* medViewerArea.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:42:58 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:14:33 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 114
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDVIEWERAREA_H
#define MEDVIEWERAREA_H

#include <QtGui>

#include <medViewerConfiguration.h>

class dtkAbstractView;

class medDataIndex;
class medToolBox;
class medViewContainer;
class medStackedViewContainers;
class medViewerAreaPrivate;

/**
 * @brief Handles all connections between all elements of the Viewer area.
 * In this Area or workspace all visualization will take place.
 * The object is instantiated by the medMainWindow to display the viewer configurations.
 *
*/
class medViewerArea : public QWidget
{
    Q_OBJECT

public:
     /**
      * @brief
      *
      * @param parent
     */
     medViewerArea(QWidget *parent = 0);
    /**
     * @brief
     *
     * @param void
    */
    ~medViewerArea(void);

    /**
     * @brief Called whenever the viewer area is switched to. Add viewer area
     *  specific status widgets on the left of the status bar.
     *
     * @param status
    */
    void setup(QStatusBar *status);

    /**
     * @brief Status bar setdown
     *
     * Called whenever the viewer area is left. Remove viewer area
     * specific status widgets from the left of the status bar.
     *
     * @param status
    */
    void setdw(QStatusBar *status);

public slots:
    /**
     * @brief Sets up the newly chosen configuration.
     *
     * When a configuration is switched to, sets up all the signal/slot connections
     * to raise the new medViewerConfiguration.
     * This calls the factory if the configuration is not yet in the private
     * hash table of configurations
     *
     * @param name the identifyer/description of the configuration.
    */
    void setupConfiguration(QString name);

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
    void open(const medDataIndex& index);

    /**
     * @brief Open data corresponding to index \param index in a new tab.
     *
     * @param index Could describe just an image, or a complete Series,
     * Study or even Patient.
     */
    void openInTab(const medDataIndex& index);

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
    void onViewClosed(void);

    /**
     * @brief  Updates the medToolBoxes when a layer (data) has been removed from a view.
     *
     * @param void
    */
    void onDataRemoved(int layer);

    /**
     * @brief Switch the view area layout to the one of patient
     * with database index \param index.
     *
     * Also sets numerous depending elements: the
     * @param index
    */
    void switchToPatient(const medDataIndex& index);


    /**
     * @brief Switches between stack of containers
     * (from one configuration to an other)
     *
     * Called from the setupConfiguration method.
     * @param stack
    */
    void switchToStackedViewContainers(medStackedViewContainers* stack);

    /**
     * @brief Switches between medViewContainers.
     *
     * The list of containers is different from one configuration to an other.
     * This method tries to switch to a container of name \param name.
     * If it doesn't exist, does nothing.
     *
     * @param name the identifyer/description of the container.
    */
    void switchToContainer(const QString& name);

    /**
     * @brief Set custom view preset
     *  Presets are defined in medViewContainerCustom::Preset.
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
     * @brief Switches from a medViewerConfiguration::LayoutType to an other.
     *
     * This can't be done by the configuration which has no access to the
     * medViewerArea Widget and its internal organisation.
     *
     * @param layout
    */
    void switchToLayout (medViewerConfiguration::LayoutType layout);

    void onFileOpened(const medDataIndex &index);
    void onFileOpenedInTab(const medDataIndex &index);

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
     * medStackedViewContainers.
     *
     * @param void
     * @return medViewContainer *
    */
    medViewContainer      *currentRootContainer(void);

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
    medViewContainer      *currentContainerFocused(void);

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
     * Allows to clear all patient related data in a medViewerConfiguration.
     * Only emitted if the setting "system/clearOnPatientChange" is set to true.
     *
    */
    void clearOnPatientChange();

private:
    friend class medMessageController;

private:
    medViewerAreaPrivate *d;
};

#endif // MEDVIEWERAREA_H
