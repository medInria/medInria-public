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

#include <QtGui/QWidget>

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
class MEDGUI_EXPORT medViewContainer : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Apart from Custom, this enum is mostly deprecated and useless.
     *
*/
    enum Type {
        Single,
        Multi,
        Custom,
        Compare,
        Fuse,
        Default
    };

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
     * @brief Gets the Type of this container.
     *
     * the default is Type::Default, and should not happen.
     *
     * @param void
     * @return Type
    */
    Type type(void);

    /**
     * @brief Gets the current container.
     *
     * This method is useful in the case of a custom or multi container.
     * Returns itself in the case of a Type::Default container or any non recursive
     * or complex container.
     *
     * @param void
     * @return medViewContainer *
    */
    medViewContainer *current(void);

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
