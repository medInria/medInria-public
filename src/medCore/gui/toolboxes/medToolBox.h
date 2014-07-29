/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtGui>

class dtkAbstractView;
class medAbstractData;

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
class MEDCORE_EXPORT medToolBox : public QWidget
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
     * @brief Gets the Toolbox's header.
     *
     * @return medToolBoxHeader *
    */
    medToolBoxHeader *header() const;

    /**
     * @brief Gets the toolbox's body.
     *
     * @return medToolBoxBody *
    */
    medToolBoxBody   *body()   const;

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

    virtual QString identifier() const {return QString();}
    virtual QString name() const {return QString();}
    virtual QString description() const {return QString();}
    virtual QStringList categories() const {return QStringList();}

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
    */
    void success();

    /**
     * @brief Emitted when an action from the toolbox failed.
     *
     * Typically used when a dtkProcess returned.
    */
    void failure();

public slots:
    /**
     * @brief Clears the toolbox.
     *
     * Resets the parameters within the tolbox,
     * for instance when the current patient changed or the view.
     *
    */
    virtual void clear();

    /**
     * @brief Switches from a minimized state to an extended one and vice versa.
     *
    */
    void switchMinimize();

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
    */
    const QStringList ValidDataTypes();

    /**
     * @brief Add a valid data type to the toolbox's valid data types.
     *
     * @param dataType
    */
    void addValidDataType(const QString & dataType);

    /**
     * @brief Show the toolbox, "overloads" the QWidget show
     *
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

#define MED_TOOLBOX_INTERFACE(_name,_desc,_categories) \
public:\
    static QString staticIdentifier() {return QString(staticMetaObject.className());}\
    static QString staticName() {return QString::fromUtf8(_name);}\
    static QString staticDescription() {return QString::fromUtf8(_desc);}\
    static QStringList staticCategories() {return QStringList() _categories;}\
    virtual QString identifier() const {return staticIdentifier();}\
    virtual QString name() const {return staticName();}\
    virtual QString description() const {return staticDescription();}\
    virtual QStringList categories() const {return staticCategories();}

