/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"

#include <QtGui>

class dtkAbstractView;
class dtkAbstractData;

class medToolBoxTab;
class medToolBoxPrivate;
class medToolBoxBody;
class medToolBoxHeader;
class dtkPlugin;


/**
 * @brief Toolbox that includes a title bar and a widget container.
 *
 * The toolboxes are displayed in the medToolBoxContainer.
 * They can be minimized, hidden, updated when a view is selected, etc...
 *
*/
class MEDGUI_EXPORT medToolBox : public QWidget
{
    Q_OBJECT

public:
    /**
    * @brief
    *
    * @param parent
    */
    medToolBox(QWidget *parent = 0);

    /**
     * @brief
     *
     * @param void
    */
    virtual ~medToolBox();

    /**
     * @brief Adds a widget to the toolbox.
     * This widget will be horizontally or vertically added to the layout given the toolbox's orientation.
     *
     * @param widget
    */
    void addWidget(QWidget *widget);

    /**
     * @brief Adds a medToolBoxTab to the toolbox.
     *
     * @param tab
    */
    void setTabWidget (medToolBoxTab* tab);

    /**
     * @brief Sets the toolbox's title.
     *
     * @param title
    */
    void setTitle(const QString& title);

    /**
     * @brief Sets the offset of the header's title from upper left corner.
     *
     * @param titleOffset
     */
    void setTitleOffset(const QPoint & titleOffset);

    /**
     * @brief Sets the orientation of the toolbox.
     * Given the orientation, lays the widgets vertically or horizontally.
     * Usually used by the medWorkspace.
     *
     * @param orientation
    */
    void setOrientation(Qt::Orientation orientation);

    /**
     * @brief Gets the toolbox's current orientation.
     *
     * @param void
     * @return Qt::Orientation
    */
    Qt::Orientation orientation() const;

    /**
     * @brief Gets the Toolbox's header.
     *
     * @param void
     * @return medToolBoxHeader *
    */
    medToolBoxHeader *header() const;

    /**
     * @brief Gets the toolbox's body.
     *
     * @param void
     * @return medToolBoxBody *
    */
    medToolBoxBody   *body()   const;

    /**
     * @brief Uncheck all buttons in this toolbox belonging to buttonGroup
     *
     * This function is called by the workspace when it receives
     * the buttonChecked signal from another toolbox.  Override it
     * when your toolbox shares a buttonGroup with other toolboxes.
     *
     * @param buttonGroup
    */
    virtual void uncheckButtons( const QString & buttonGroup ) {}


    /**
     * @brief Enables or disable the aboutPlugin button.
     *
     * @param enable true to display the button.
     */
    void setAboutPluginVisibility(bool enable);

    /**
     * @brief Returns the aboutPlugin button visibility.
     *
     */
    bool aboutPluginVisibility();

    /**
     * @brief Sets up the plugin this button is refering to.
     *
     * @param plugin The dtkPlugin this button will give info about.
     */
    void setAboutPluginButton(dtkPlugin * plugin);

signals:
    /**
     * @brief Tells the world to add a new toolbox to the medToolboxContainer.
     * Typically used when a generic toolbox adds a custom toolbox.
     *
     * @param toolbox
    */
    void addToolBox(medToolBox *toolbox);

    /**
     * @brief Tells the world to remove a toolbox from the medToolBoxContainer.
     *
     * @param toolbox
    */
    void removeToolBox(medToolBox *toolbox);

    /**
     * @brief Emitted when an action from the toolbox succeeded.
     * Typically used when a dtkProcess returned.
     *
     * @param void
    */
    void success();

    /**
     * @brief Emitted when an action from the toolbox failed.
     *
     * Typically used when a dtkProcess returned.
     * @param void
    */
    void failure();

    /**
     * @brief Use this if buttons in different toolboxes are mutually exclusive
     *
     * Emit this signal to uncheck buttons of the same buttonGroup in
     * other toolboxes.
     *
     * @param buttonGroup
    */
    void buttonChecked( const QString & buttonGroup );

public slots:
    /**
     * @brief Clears the toolbox.
     *
     * Resets the parameters within the tolbox,
     * for instance when the current patient changed or the view.
     *
     * @param void
    */
    virtual void clear();

    /**
     * @brief Updates the content of the toolbox when a new view is selected.
     *
     * @param view The view to extract information from.
    */
    virtual void update(dtkAbstractView *view);

    /**
     * @brief Switches from a minimized state to an extended one and vice versa.
     *
    */
    void switchMinimize();

    /*
     * Remove not context meaningful toolboxes.
     *
    */

    /**
     * @brief Set toolbox's valid data types.
     *
     * If at least one datatype in the view is contained within the
     * toolBoxValidTypes the toolbox will be visible. If the list is empty,
     * then the toolbox will be context free, and always visible whatever the
     * focused view.
     *
     * @param dataTypes Valid data types of the toolbox.
    */
    void setValidDataTypes(const QStringList & dataTypes);

    /**
     * @brief Get toolbox's valid data types.
     *
     * @see setValidDataTypes
     * @param void
    */
    const QStringList ValidDataTypes();

    /**
     * @brief Add a valid data type to the toolbox's valid data types.
     *
     * @param dataType
    */
    void addValidDataType(const QString & dataType);


     /**
      * @brief Set the toolbox visibility.
      *
      * If at least one datatype in the viewDataTypes is contained within the
      * toolBox's ValidTypes, or if the validDataTypes are empty, the toolbox
      * will be visible.
      *
      * @param viewDataTypes
     */
    void setContextVisibility(const QHash<QString, unsigned int> & viewDataTypes);

    /**
     * @brief Access method to the toolBox context visibility flag.
     *
     * @param void
    */
    bool ContextVisible();

    /**
     * @brief Show the toolbox, "overloads" the QWidget show
     *
     * @param void
    */
    void show();


protected slots:
    /**
     * @brief Raises a dtkAboutPlugin dialog.
     *
     */
    void onAboutButtonClicked();
    
private:
    medToolBoxPrivate *d;
};


