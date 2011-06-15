/* medViewerConfiguration.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 14:11:43 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 14:22:37 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWERCONFIGURATION_H
#define MEDVIEWERCONFIGURATION_H

#include <QtCore>
#include "medViewContainer.h"
#include "medViewContainerCustom.h"

#include "medGuiExport.h"

class medToolBox;
class medViewerConfigurationPrivate;
class medStackedViewContainers;
class medDataIndex;

/**
 * @brief A Configuration holds medToolBoxes, medViewContainers and their relations.
 * 
 *The main role of a configuration is to provide a coherent set of toolboxes and containers that interact with each other.
 *
 * A configuration is usually instantiated by a factory. 
 * It owns several medViewContainers in a medStackedViewContainers. 
 * It also owns toolboxes, but does not place them, the medViewerArea does it when
 * medViewerArea::setupConfiguration is called. 
 *
*/
class MEDGUI_EXPORT medViewerConfiguration : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 
     *
     * @param parent
    */
    medViewerConfiguration(QWidget *parent=0);
    /**
     * @brief 
     *
     * @param void
    */
    ~medViewerConfiguration(void);
    
    /**
     * @brief Identifies a configuration in the factory.
     * @todo split this description with a human readable one.
     *
     * @param void
     * @return QString
    */
    virtual QString description(void) const = 0;
    
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
    QList<medToolBox*> toolBoxes(void) const;
    
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
    LayoutType layoutType(void) const;
    
    /**
     * @brief Switches to an other container.
     *
     * @param name
    */
    void setCurrentViewContainer(const QString& name);
    
    /**
     * @brief Sets the layout preset for medViewContainerCustom.
     *
     * The value is really applied when the configuration 
     * is set in the medViewArea::setupConfiguration method.
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
    int customLayoutPreset(void) const;
    
    /**
     * @brief Sets the Database navigator's visibility.
     *
     * The value is really applied when the configuration 
     * is set in the medViewArea::setupConfiguration method.
     * @param bool
    */
    void setDatabaseVisibility(bool);
    
    /**
     * @brief Gets the database navigator's visibility. 
     *
     * @param void
     * @return bool
    */
    bool isDatabaseVisible(void) const;

    /**
     * @brief Sets the toolbox containers's visibility.
     *
     * The value is really applied when the configuration 
     * is set in the medViewArea::setupConfiguration method.
     * @param bool
    */
    void setToolBoxesVisibility(bool);
    
    /**
     * @brief Gets the toolbox container's visibility.
     *
     * @param void
     * @return bool
    */
    bool areToolBoxesVisible(void) const;

    /**
     * @brief Sets the visibility of the medViewerToolBoxLayout.
     *
     * The value is really applied when the configuration 
     * is set in the medViewArea::setupConfiguration method.
     * @param visibility
    */
    void setLayoutToolBoxVisibility(bool visibility);
    
    /**
     * @brief Hides the medViewerToolBoxLayout.
     *
     * @param void
    */
    void hideLayoutToolBox(void);
    
    /**
     * @brief Show the medViewerToolBoxLayout.
     *
     * @param void
    */
    void showLayoutToolBox(void);
    
    /**
     * @brief Gets the visibility of the medViewerToolBoxLayout.
     *
     * @return bool
    */
    bool isLayoutToolBoxVisible() const;
    
    /**
     * @brief Sets up the medStackedViewContainers.
     *
     * Abstract method that every configuration should implement. It sets or 
     * resets the stack so that containers are connected properly with the toolboxes,
     * between them if needed, and without any dangling views.
    */
    virtual void setupViewContainerStack()=0;

    /**
     * @brief Convenience method to add a medViewContainerSingle.
     *
     * @param name Identifyer/description. By Default "Single", 
     * which makes sense in most simple cases.
    */
    void addSingleContainer(const QString& name="Single");
    
    /**
     * @brief Convenience method to add a medViewContainerMulti.
     *
     * @param name Identifyer/description. By Default "Multi", 
     * which makes sense in most simple cases.
    */
    void addMultiContainer(const QString& name="Multi");
    
    /**
     * @brief Convenience method to add a medViewContainerCustom.
     *
     * @param name Identifyer/description. By Default "Custom", 
     * which makes sense in most simple cases.
    */
    void addCustomContainer(const QString& name="Custom");
    
    /**
     * @brief Gets the currently displayed container.
     *
     * @return medViewContainer *
    */
    medViewContainer* currentViewContainer() const;
    
    /**
     * @brief Gets the description/identifyer of the currently displayed container.
     *
     * @return QString
    */
    QString currentViewContainerName() const;
    
    /**
     * @brief Gets the stack of Containers.
     *
     * @return medStackedViewContainers *
    */
    medStackedViewContainers * stackedViewContainers() const;
    
public slots:
    /**
     * @brief Adds a toolbox to the configuration.
     *
     * This function connects the medToolBox::addToolBox signals 
     * from a new toolbox with the toolboxAdded signal of the configuration.
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
     * @brief Removes a toolbox from the configuration.
     *
     * Disconnects the signals connected at addition time, 
     * and emits a toolboxRemoved signal.
     *
     * @see addToolBox
     * @param toolbox
    */
    virtual void removeToolBox(medToolBox *toolbox);
    
    /**
     * @brief Clears the Configuration.
     *
     * Gives the configuration a pristine look and state.
     * This method removes the containers 
     * (by calling medStackedViewContainers::removeContainer)
     *
    */
    virtual void clear();
    
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
     * This signal is mainly intended for the medViewerArea, 
     * that connects it with its medViewerArea::switchToContainer slot.
     * @todo rename this signal when changing the medViewerToolBoxLayout API.
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
    
protected:
    /**
     * @brief Clears the configuration's toolboxes.
     *
     * resets all the buttons and internal parameters of the Configuration.
     *
    */
    void clearToolBoxes();
    
private:
    medViewerConfigurationPrivate *d;
};

#endif
