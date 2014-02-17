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

#include <medGuiExport.h>

class QUuid;

class medToolBox;

class medTabbedViewContainers;
class medDataIndex;
class medViewContainer;

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
class medAbstractWorkspacePrivate;
class MEDGUI_EXPORT medAbstractWorkspace : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     * @param parent
    */
    medAbstractWorkspace(QWidget *parent=0);
    /**
     * @brief
     *
     * @param void
    */
    ~medAbstractWorkspace();

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

    void addWorkspaceToolBox(medToolBox *toolbox);
    void removeWorkspaceToolBox(medToolBox *toolbox);
    QList <medToolBox*> workspaceToolBoxes() const;
    medToolBox* containerToolBox() const;

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
     * @brief Gets the currently displayed container.
     *
     * @return medViewContainer *
    */
    medViewContainer* currentViewContainer() const;

    /**
     * @brief Gets the stack of Containers.
     *
     * @return medTabbedViewContainers *
    */
    medTabbedViewContainers * stackedViewContainers() const;

public slots:

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
    virtual void addNewTab();


    /**
     * @brief Switches to an other container.
     *
     * @param name
    */
    void setCurrentViewContainer(QUuid &uuid);


    void addNewContainer(QUuid& uuid);

    void removeContainer(QUuid& uuid);

signals:
    void currentContainerChanged();
    void currentViewChanged();

protected:
    /**
     * @brief Clears the workspace's toolboxes.
     *
     * resets all the buttons and internal parameters of the Workspace.
     *
    */
    void clearWorkspaceToolBoxes();

private:
    medAbstractWorkspacePrivate *d;
};


