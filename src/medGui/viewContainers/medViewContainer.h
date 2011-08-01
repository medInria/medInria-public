/* medViewContainer.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 26 21:53:58 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Jun 15 16:21:52 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 32
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWCONTAINER_H
#define MEDVIEWCONTAINER_H

#include "medGuiExport.h"

#include <QtGui/QFrame>

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
    Q_PROPERTY( bool clicked READ isClicked );

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
    ~medViewContainer(void);

    

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
    const medViewContainer *current(void) const;
    medViewContainer *current(void);

     /**
     * @brief Does this container has focus?
     *
     * Returns true if it does.
     *
     * @param void
     * @return bool
    */
    virtual bool isClicked(void) const;

    /**
     * @brief Is this the current container?
     *
     * Returns true if this->current() returns this.
     *
     * @param void
     * @return bool
    */
    virtual bool isCurrent(void) const;

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
    virtual bool isRoot(void) const;

    /**
     * @brief Is this a leaf container?
     *
     * Returns true if this object is supposed to contain only views
     * and no other containers.
     *
     * @param void
     * @return bool
    */
    virtual bool isLeaf(void) const;

    /**
     * @brief Is this container empty?
     *
     * Returns true if this object contains no other view containers
     * or views.
     *
     * @param void
     * @return bool
    */
    virtual bool isEmpty(void) const;

    /**
     * @brief Does this container contain a daddy view?
     *
     * Returns true if this object contains a daddy view.
     *
     * @param void
     * @return bool
    */
    virtual bool isDaddy(void) const;

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
    medViewPool *pool (void);

    /**
     * @brief Sets the view of this container.
     * Also passes the viewProperties to the view.
     *
     *
     * @param view
    */
    virtual void setView(dtkAbstractView *view);
    
    /**
     * @brief Gets the view associated with the container.
     *
     * @param void
     * @return dtkAbstractView *
    */
    virtual dtkAbstractView         *view  (void) const;
    
    /**
     * @brief Gets the views contained here.
     *
     * If there is just one view (accessible by getView), returns a list with that
     * one element.
     *
     * @param void
     * @return QList<dtkAbstractView *>
    */
    virtual QList<dtkAbstractView *> views (void) const;

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
    QString info(void);
    
signals:
    /**
     * @brief A new medDataIndex has been dropped onto the container.
     *
     * @param index
    */
    void dropped(const medDataIndex& index);
    
    /**
     * @brief The focus has been placed on the view this container is hosting.
     *
     * @param view
    */
    void focused(dtkAbstractView *view);

    /**
     * @brief The container has been clicked.
    */
    void clicked (void);
    
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
    virtual void onViewFocused( bool value );

    /**
     * @brief This slot is called when another container has been clicked.
     * The current container changes its "clicked" property to false.
    */
    virtual void onContainerClicked (void);


protected:
    /**
     * @brief Sets the currently selected Container.
     *
     * sets it to the parent Container if there is a parent. 
     * If the container has no parent, then the current container is set concretely. 
     *
     * @param container
    */
    void setCurrent(medViewContainer *container);

    /**
     * @brief Recompute the style sheet after a property change
     *
     * The appearance of the border is controlled by the style sheet
     * (medinria.qss) and depends on the values of properties like
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

protected:
    medViewContainerPrivate *d;
};

#endif // MEDVIEWCONTAINER_H
