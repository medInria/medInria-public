/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medGuiExport.h"

#include <QtGui/QFrame>

class dtkAbstractData;
class dtkAbstractView;
class medDataIndex;
class medViewPool;

class medViewContainerPrivate;

/**
 * @brief A viewContainer is a QWidget hosting one or several dtkAbstractViews.
 *
 *
 *
*/
class MEDGUI_EXPORT medViewContainer : public QFrame
{
    Q_OBJECT

    /**
     * @brief true if this container has focus
    */
    Q_PROPERTY( bool selected READ isSelected );

    /**
     * @brief true if this is the current container
    */
    Q_PROPERTY( bool isCurrent READ isCurrent );

    /**
     * @brief true if this is the root container
     *
     * The root container is not contained in another view container,
     * it may contain other view containers, if it is multi or custom
     * view.
    */
    Q_PROPERTY( bool isRoot READ isRoot );

    /**
     * @brief true if this is a leaf container
     *
     * The leaf container doesn't contain any other view container, it
     * may be contained in another view container.
    */
    Q_PROPERTY( bool isLeaf READ isLeaf );

    /**
     * @brief true if this container contains no other containers or views
     *
     * The leaf container doesn't contain any other view, it may be
     * contained in another view container.
    */
    Q_PROPERTY( bool isEmpty READ isEmpty );

    /**
     * @brief true if this container contains a daddy view
    */
    Q_PROPERTY( bool isDaddy READ isDaddy );

public:

     /**
      * @brief Creates a new container.
      *
      * Sets a few default values: it accepts drops,
      * gets focus on click (Qt::ClickFocus).
      *
      * @param parent
     */
     medViewContainer(QWidget *parent = 0);

    /**
     * @brief
     *
     * @param void
    */
    ~medViewContainer();



    /**
     * @brief Gets the current container.
     *
     * This method is useful in the case of a custom or multi
     * container.  Returns itself in the case of a Type::Default
     * container or any non recursive or complex container.
     *
     * @param void
     * @return medViewContainer *
    */
    const medViewContainer *current() const;
    medViewContainer *current();

     /**
     * @brief Is this container selected?
     *
     * Returns true if it is.
     *
     * @param void
     * @return bool
    */
    virtual bool isSelected() const;

    /**
     * @brief Is this the current container?
     *
     * Returns true if this->current() returns this.
     *
     * @param void
     * @return bool
    */
    virtual bool isCurrent() const;

    /**
     * @brief Is this the root container?
     *
     * Returns true if this object is not contained in another view
     * container.  This method is useful particularly in the case of a
     * custom or multi container.
     *
     * @param void
     * @return bool
    */
    virtual bool isRoot() const;

    /**
     * @brief Is this a leaf container?
     *
     * Returns true if this object is supposed to contain only views
     * and no other containers.
     *
     * @param void
     * @return bool
    */
    virtual bool isLeaf() const;

    /**
     * @brief Is this container empty?
     *
     * Returns true if this object contains no other view containers
     * or views.
     *
     * @param void
     * @return bool
    */
    virtual bool isEmpty() const;

    /**
     * @brief Does this container contain a daddy view?
     *
     * Returns true if this object contains a daddy view.
     *
     * @param void
     * @return bool
    */
    virtual bool isDaddy() const;

    /**
     * @brief Get the parent container
     *
     * Returns the parent in the container hierarchy, or NULL if this
     * is the root.  This method is useful particularly in the case of
     * a custom or multi container.
     *
     * @param void
     * @return medViewContainer *
    */
    medViewContainer * parentContainer() const;

    /**
     * @brief Get the root container
     *
     * Returns the root of the container hierarchy.  This method is
     * useful particularly in the case of a custom or multi container.
     *
     * Since the return value might be a pointer to 'this', the const
     * version of the function must return a const pointer.
     *
     * @param void
     * @return medViewContainer *
    */
    const medViewContainer * root() const;
    medViewContainer * root();

    /**
     * @brief Get the direct child containers
     *
     * Returns a list of the containers contained in this one,
     * regardless if they are leaves or not.  This method is useful
     * particularly in the case of a custom or multi container.
     *
     * @param void
     * @return QList< medViewContainer * >
    */
    QList< medViewContainer * > childContainers() const;

    /**
     * @brief Get the leaf containers
     *
     * Returns a list the leaf containers which are direct or indirect
     * children of this one.  If this is a leaf container, the
     * returned list contains just this.  If excludeEmpty is true,
     * only leaves containing a view are returned.
     *
     * @param excludeEmpty
     * @return QList< medViewContainer * >
    */
    // QList< medViewContainer * > leaves( bool excludeEmpty = false ) const;
    QList< medViewContainer * > leaves( bool excludeEmpty = false );

    /**
     * @brief Gets the medViewPool associated with this container.
     *
     * @param void
     * @return medViewPool *
    */
    medViewPool *pool();

    /**
     * @brief Sets the view of this container.
     *
     * Also passes the viewProperties to the view.
     * This method is meant to be overloaded in inherited classes with the specific behaviour.
     *
     * It is the responsibility of this method to give focus to the container.
     * Method overriding this MUST call setFocus(Qt::MouseFocusReason);
     * and NOT onFocused()
     * since this method doesn't deal with changing from the old container
     * to the new one if needed.
     *
     * If the container is not a leaf, and therefore doesn't have a view,
     * this method is in charge of setting the view of the current container.
     *
     * @param view
    */
    virtual void setView(dtkAbstractView *view);

    /**
     * @brief Gets the view associated with the container.
     *
     * If the container is not a leaf, returns a pointer to an editable view
     * in the current child container.
     * If the container is multi for instance, it will return NULL to this request, there is no editable view in Multi.

     * Use this method to get a pointer to a view in which to drop some data
     *
     * @param void
     * @return dtkAbstractView * or NULL if there is no editable view attached to this container, or its current leaf.
    */
    virtual dtkAbstractView         *view() const;

    /**
     * @brief Gets the views contained here.
     *
     * If there is just one view (accessible by getView), returns a list with that
     * one element.
     *
     * @param void
     * @return QList<dtkAbstractView *>
    */
    virtual QList<dtkAbstractView *> views() const;

    /**
     * @brief Gets the value of from a viewProperty key.
     *
     * @param key
     * @return QString
    */
    virtual QString viewProperty (const QString &key) const;

    /**
     * @brief Sets an information string to the container.
     *
     * This string is then displayed in the container's background
     * when there is no view.
     *
     * @param info
    */
    void setInfo(const QString& info);

    /**
     * @brief Gets the info string.
     *
     * @param void
     * @return QString
    */
    QString info();

    virtual QString identifier() {return "";}
    virtual QString description() {return "";}

    void setMultiLayer( bool enable = true);
    virtual bool multiLayer();
    
    /**
     * @brief Opens data at corresponding index using DataManager and creates a new view 
     * or adds data to the current view if multiLayer is enabled. 
     * Index must be valid for serie. 
     *
     * @param index
     * @return bool
    */
    bool open(const medDataIndex& index);
     
    /**
     * @brief Creates a new view  or adds data to the current view if multiLayer is enabled. 
     *
     * @param data
     * @return bool
    */
    bool open(dtkAbstractData * data);


    void emitImageSet(const medDataIndex& index);


signals:
    /**
     * @brief A new medDataIndex has been set onto the container.
     *
     * @param index
    */
    void imageSet(const medDataIndex& index);

    /**
     * @brief The focus has been placed on the view this container is hosting.
     *
     * @param view
    */
    void focused(dtkAbstractView *view);

    /**
     * @brief The container has been selected.
    */
    void selected();

    /**
     * @brief A view has been added to the container.
     *
     * @param view
    */
    void viewAdded   (dtkAbstractView *view);

    /**
     * @brief A view has been added from the container.
     *
     * @param view
    */
    void viewRemoved (dtkAbstractView *view);

    /**
     * @brief A slice has been selected for visualization.
     *
     * @param slice
    */
    void sliceSelected( int slice );

public slots:
    /**
     * @brief
     *
     * @param rows
     * @param cols
    */
    virtual void split(int rows, int cols);

    /**
     * @brief Sets a view property.
     *
     * Properties are set to new views,
     * and displayed in the Container's background when no view are set.
     *
     * @param key
     * @param value
    */
    virtual void setViewProperty (const QString &key, const QString &value);

    /**
     * @brief
     *
     * @param value
    */
    virtual void onViewFullScreen (bool value);

    /**
     * @brief
     *
     * @param value
    */
    virtual void onDaddyChanged( bool state );

    /**
     * @brief
     *
     * @param value
    */
    virtual void select();

    /**
     * @brief This slot is called when another container has been clicked.
     * The current container changes its "selected" property to false.
    */
    virtual void unselect();


protected:
    /**
     * @brief Sets the currently selected Container.
     *
     * Set to the container that has focus currently. When Focus is changed
     * it is the responsibililty of the leaf container to change the current
     * container of its parent to the new one.
     *
     * @param container
    */
    void setCurrent(medViewContainer *container);

    /**
     * @brief Recompute the style sheet after a property change
     *
     * The appearance of the border is controlled by the style sheet
     * (medInria.qss) and depends on the values of properties like
     * isCurrent, isLeaf, or isEmpty.  However, if the value of a
     * property changes after the style sheet has been set, it is
     * necessary to force a style sheet recomputation.  One way to
     * achieve this is to unset the style sheet and set it again.
     *
     * @param void
    */
    void recomputeStyleSheet();

protected:
    /**
     * @brief
     *
     * @param event
    */
    void dragEnterEvent(QDragEnterEvent *event);

    /**
     * @brief
     *
     * @param event
    */
    void dragMoveEvent(QDragMoveEvent *event);

    /**
     * @brief
     *
     * @param event
    */
    void dragLeaveEvent(QDragLeaveEvent *event);

    /**
     * @brief Parses a drop event, translating it into a medDataIndex.
     *
     * @param event
    */
    void dropEvent(QDropEvent *event);

    /**
     * @brief Container got Focus.
     *
     * Sets it as current, focuses the view if it exists,
     * updates the previous current Container and the new current one.
     * @todo should not update the previous current container: should be done in focusOutEvent();
     * @param event
    */
    void focusInEvent(QFocusEvent *event);

    /**
     * @brief Container lost focus.
     *
     * @param event
    */
    void focusOutEvent(QFocusEvent *event);

    /**
     * @brief
     *
     * @param event
    */
    void paintEvent(QPaintEvent *event);

    bool eventFilter(QObject * obj, QEvent *ev);

protected:
    medViewContainerPrivate *d;
};


