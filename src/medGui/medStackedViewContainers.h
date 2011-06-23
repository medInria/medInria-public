/* medStackedViewContainers.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 12:37:58 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 12:39:00 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWCONTAINERSTACK_H
#define MEDVIEWCONTAINERSTACK_H

#include <QtGui>

#include "medGuiExport.h"

class dtkAbstractView;
class medDataIndex;
class medViewContainer;
class medStackedViewContainersPrivate;

/**
 * @brief A QStackedWidget that contains medViewContainers.
 * There is one such stack per medViewConfiguration. 
 *
*/
class MEDGUI_EXPORT medStackedViewContainers : public QTabWidget
{
    Q_OBJECT

public:
     /**
      * @brief 
      *
      * @param parent
     */
     medStackedViewContainers(QWidget *parent = 0);
     
    /**
     * @brief 
     *
     * @param void
    */
    ~medStackedViewContainers(void);

    /**
     * @brief Gets the currently displayed container.
     *
     * @param void
     * @return medViewContainer *
    */
    medViewContainer *current(void) const;
    
    /**
     * @brief Gets the name of the current container.
     * Containers are referenced by name in a private hash table
     *
     * @param void
     * @return QString
    */
    QString currentName(void) const;

    /**
     * @brief Adds a container to the stack.
     *
     * @param name The name that will identify this container in the stack.
     * @param container
    */
    void addContainer(const QString &name, medViewContainer *container);

    /**
     * @brief Inserts a container in the stack.
     *
     * @param index The index in the tab widget where to insert the container.
     * @param name The name that will identify this container in the stack.
     * @param container
    */
    void insertContainer(int index, const QString &name, medViewContainer *container);

    /**
     * @brief Gets the container identified in the stack by this string.
     *
     * @param name
     * @return medViewContainer * the container or NULL the name does not match.
    */
    medViewContainer* container(const QString &name) const;
    
    /**
     * @brief Puts the container with this name on top of the stack.
     * Does nothing if the name is not contained.
     * @param name
    */
    void setContainer(const QString &name);

    
    /**
     * @brief Gets the list of containers in the stack.
     *
     * @return QList<QString>
    */
    QList<QString> keys();
    
    /**
     * @brief Removes one container from the stack.
     *
     * @param name
    */
    void removeContainer(const QString& name);

    void lockTabs();
    void unlockTabs();

signals:
    /**
     * @brief Emits this signal when one of the containers has emitted 
     * the same signal.
     *
     * @param 
    */
    void dropped(const medDataIndex&);
    
    /**
     * @brief Emits this signal when one of the containers has emitted 
     * the same signal.
     *
     * @param 
    */
    void focused(dtkAbstractView *);

public slots:
    void changeCurrentContainerType(const QString &name);
    void addNewTabContainer();
    void deleteContainerClicked(int index);

private:
    medStackedViewContainersPrivate *d;
};

#endif
