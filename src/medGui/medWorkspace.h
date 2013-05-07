/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>
#include "medViewContainer.h"
#include "medCustomViewContainer.h"

#include "medGuiExport.h"

class medToolBox;
class medWorkspacePrivate;
class medTabbedViewContainers;
class medDataIndex;

/**
 * @brief A Workspace holds medToolBoxes, medViewContainers and their relations.
 *
 *The main role of a workspace is to provide a coherent set of toolboxes and containers that interact with each other.
 *
 * A workspace is usually instantiated by a factory.
 * It owns several medViewContainers in a medTabbedViewContainers.
 * It also owns toolboxes, but does not place them, the medWorkspaceArea does it when
 * medWorkspaceArea::setupWorkspace is called.
 *
*/
class MEDGUI_EXPORT medWorkspace : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     * @param parent
    */
    medWorkspace(QWidget *parent=0);
    /**
     * @brief
     *
     * @param void
    */
    ~medWorkspace();

    /**
     * @brief Identifies a workspace in the factory.
     *
     * @param void
     * @return QString
    */
    virtual QString identifier() const = 0;

    virtual QString description() const = 0;

    /**
     * @brief Describes the layout of the workspace.
     *
     * The containers are always between the database navigator and the toolboxes.
     *
    */
    enum LayoutType {
        LeftDbRightTb, /**database on the left, Toolboxes on the right */
        LeftTbRightDb, /**Toolboxes on the left, Database on the right*/
        TopDbBottomTb, /**database on top, Toolboxes below */
        TopTbBottomDb  /**database below, Toolboxes on top */
    };


    /**
     * @brief Gets the list of toolboxes' names.
     *
     * @param void
     * @return QList<medToolBox *>
    */
    QList<medToolBox*> toolBoxes() const;

    /**
     * @brief Sets the LayoutType.
     *
     * Only four are supported right now. They describe the positions of the navigator and the toolboxes in regards with the Containers.
     * @param type
    */
    void setLayoutType(LayoutType type);

    /**
     * @brief Gets the LayoutType.
     *
     * @see setLayoutType
     * @param void
     * @return LayoutType
    */
    LayoutType layoutType() const;

    /**
     * @brief Switches to an other container.
     *
     * @param name
    */
    void setCurrentViewContainer(const QString& name);

    /**
     * @brief Sets the layout preset for medCustomViewContainer.
     *
     * The value is really applied when the workspace
     * is set in the medViewArea::setupWorkspace method.
     *
     * @param value
    */
    void setCustomPreset(int value);

    /**
     * @brief
     *
     * @param void
     * @return int
    */
    int customLayoutPreset() const;

    /**
     * @brief Sets the Database navigator's visibility.
     *
     * The value is really applied when the workspace
     * is set in the medViewArea::setupWorkspace method.
     * @param bool
    */
    void setDatabaseVisibility(bool);

    /**
     * @brief Gets the database navigator's visibility.
     *
     * @param void
     * @return bool
    */
    bool isDatabaseVisible() const;

    /**
     * @brief Sets the toolbox containers's visibility.
     *
     * The value is really applied when the workspace
     * is set in the medViewArea::setupWorkspace method.
     * @param bool
    */
    void setToolBoxesVisibility(bool);

    /**
     * @brief Gets the toolbox container's visibility.
     *
     * @param void
     * @return bool
    */
    bool areToolBoxesVisible() const;

    /**
     * @brief Sets up the medTabbedViewContainers.
     *
     * Abstract method that every workspace should implement. It sets or
     * resets the stack so that containers are connected properly with the toolboxes,
     * between them if needed, and without any dangling views.
    */
    virtual void setupViewContainerStack()=0;

    /**
     * @brief Convenience method to add a container of a type defined in the
     * settings.
     *
     * The parameter in the settings is: ("startup","default_container_layout").
     * Values are "Single", "Multi", or "Custom"
     *
     * @param name identifier/description. If the string is empty, Defaults to :
     * "Single", "Multi", or "Custom"
     * which makes sense in most simple cases.
     * @return name of the created container
    */
    QString addDefaultTypeContainer(const QString& name = "");

    /**
     * @brief Convenience method to add a medSingleViewContainer.
     *
     * @param name identifier/description. By Default "Single",
     * which makes sense in most simple cases.
     * @return name of the created container
    */
    QString addSingleContainer(const QString& name="Single");

    /**
     * @brief Convenience method to add a medMultiViewContainer.
     *
     * @param name identifier/description. By Default "Multi",
     * which makes sense in most simple cases.
     * @return name of the created container
    */
    QString addMultiContainer(const QString& name="Multi");

    /**
     * @brief Convenience method to add a medCustomViewContainer.
     *
     * @param name identifier/description. By Default "Custom",
     * which makes sense in most simple cases.
     * @return name of the created container
    */
    QString addCustomContainer(const QString& name="Custom");

    /**
     * @brief Gets the currently displayed container.
     *
     * @return medViewContainer *
    */
    medViewContainer* currentViewContainer() const;

    /**
     * @brief Gets the description/identifier of the currently displayed container.
     *
     * @return QString
    */
    QString currentViewContainerName() const;

    /**
     * @brief Gets the stack of Containers.
     *
     * @return medTabbedViewContainers *
    */
    medTabbedViewContainers * stackedViewContainers() const;

public slots:
    /**
     * @brief Adds a toolbox to the workspace.
     *
     * This function connects the medToolBox::addToolBox signals
     * from a new toolbox with the toolboxAdded signal of the workspace.
     * This way the medToolBoxContainer will eventually be notified and display
     * new toolboxes added by the one in parameter here.
     * A toolboxAdded signal is also emitted to add this toolbox
     * to the medToolBoxContainer as well.
     * @see medToolBox::addToolBox
     *
     * @param toolbox
    */
    virtual void addToolBox(medToolBox *toolbox);

    /**
     * @brief Removes a toolbox from the workspace.
     *
     * Disconnects the signals connected at addition time,
     * and emits a toolboxRemoved signal.
     *
     * @see addToolBox
     * @param toolbox
    */
    virtual void removeToolBox(medToolBox *toolbox);

    /**
     * @brief Clears the Workspace.
     *
     * Gives the workspace a pristine look and state.
     * This method removes the containers
     * (by calling medTabbedViewContainers::removeContainer)
     *
    */
    virtual void clear();

    /**
      * @brief Adds a new tab to a workspace
      *
      * Default implementation adds a multi-container tab
      * If another behavior is wanted, override this in child class
      */
    virtual void onAddTabClicked();

    /**
      * @brief Adapt interface to container change in stacks
      *
      * E.g. changes the layout toolbox to set it to the current container type
      */
    virtual void onContainerChanged(const QString &name);

    /**
     * @brief Tells all toolboxes that a button of buttonGroup has been checked
     *
     * All toolboxes except the sender of the buttonChecked signal are
     * told to uncheck their buttons belonging to the same
     * buttonGroup.  This mechanism can be used when buttons in
     * different toolboxes are mutually exclusive.
     *
     * @param buttonGroup
     */
    void onButtonChecked( const QString & buttonGroup );

signals:
    /**
     * @brief Tells the world a new toolbox should be added to its medToolBoxContainer.
     *
     * @param tb
    */
    void toolboxAdded  (medToolBox *tb);

    /**
     * @brief Tells the world a toolbox should be removed from its medToolBoxContainer.
     *
     * @param tb
    */
    void toolboxRemoved(medToolBox *tb);

    /**
     * @brief Tells the world the layout toolbox has changed the container.
     *
     * This signal is mainly intended for the medWorkspaceArea,
     * that connects it with its medWorkspaceArea::switchToContainer slot.
     * @todo rename this signal when changing the medVisualizationLayoutToolBox API.
     *
     * @param mode
    */
    void layoutModeChanged (const QString& mode);

    /**
     * @brief
     *
     * @param int
     * @param int
    */
    void layoutSplit(int, int);

    /**
     * @brief
     *
     * @param int
    */
    void layoutPresetClicked(int);

    /**
     * @brief Signal to change layout toolbox tab type
     *
     * Connect it in your workspace to the layout toolbox
     *
     * @param int
    */
    void setLayoutTab(const QString &);

protected:
    /**
     * @brief Clears the workspace's toolboxes.
     *
     * resets all the buttons and internal parameters of the Workspace.
     *
    */
    void clearToolBoxes();

private:
    medWorkspacePrivate *d;
};


