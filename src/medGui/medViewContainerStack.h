/* medViewContainerStack.h --- 
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
class medViewContainerStackPrivate;

/**
 * @brief 
 *
*/
class MEDGUI_EXPORT medViewContainerStack : public QStackedWidget
{
    Q_OBJECT

public:
     /**
      * @brief 
      *
      * @param parent
     */
     medViewContainerStack(QWidget *parent = 0);
     
    /**
     * @brief 
     *
     * @param void
    */
    ~medViewContainerStack(void);

    /**
     * @brief 
     *
     * @param void
     * @return medViewContainer *
    */
    medViewContainer *current(void) const;
    
    /**
     * @brief 
     *
     * @param void
     * @return QString
    */
    QString currentName(void) const;
    
    /**
     * @brief 
     *
     * @param name
     * @param container
    */
    void addContainer(const QString &name, medViewContainer *container);
    
    /**
     * @brief 
     *
     * @param name
     * @return medViewContainer *
    */
    medViewContainer* container(const QString &name) const;
    
    /**
     * @brief 
     *
     * @param name
    */
    void setContainer(const QString &name);

signals:
    /**
     * @brief 
     *
     * @param 
    */
    void dropped(const medDataIndex&);
    
    /**
     * @brief 
     *
     * @param 
    */
    void focused(dtkAbstractView *);

private:
    medViewContainerStackPrivate *d;
};

#endif
