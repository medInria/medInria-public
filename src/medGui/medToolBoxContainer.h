/* medToolBoxContainer.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 26 21:50:02 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Oct 26 22:01:36 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include "medGuiExport.h"

#include <QtGui/QScrollArea>

class medToolBox;
class medToolBoxContainerPrivate;

class MEDGUI_EXPORT medToolBoxContainer : public QScrollArea
{
    Q_OBJECT
public:
     medToolBoxContainer(QWidget *parent = 0);
    ~medToolBoxContainer(void);

    void addToolBox(medToolBox *toolBox);
    void removeToolBox(medToolBox *toolBox);
    void clear(void);
    
    QList<medToolBox*> toolBoxes(void) const;

    Qt::Orientation orientation (void) const;

    /**
     * @brief Set margin and spacing of the layout
     *
     * @param spacing
    */
    void setLayoutSpacing(int spacing);
    
public slots:
    void setOrientation(Qt::Orientation orientation);

private:
    medToolBoxContainerPrivate *d;
};


