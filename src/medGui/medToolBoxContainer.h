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

#ifndef MEDTOOLBOXCONTAINER_H
#define MEDTOOLBOXCONTAINER_H

#include "medGuiExport.h"

#include <QtGui/QScrollArea>

class medToolBox;
class medToolBoxContainerPrivate;

class MEDGUI_EXPORT medToolBoxContainer : public QScrollArea
{
public:
     medToolBoxContainer(QWidget *parent = 0);
    ~medToolBoxContainer(void);

    void addToolBox(medToolBox *toolBox);
    void removeToolBox(medToolBox *toolBox);
    void clear(void);

private:
    medToolBoxContainerPrivate *d;
};

#endif // MEDTOOLBOXCONTAINER_H
