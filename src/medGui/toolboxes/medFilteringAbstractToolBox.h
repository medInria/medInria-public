/* medFilteringAbstractToolBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 14:16:25 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:46:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 23
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDTOOLBOXFILTERINGCUSTOM_H
#define MEDTOOLBOXFILTERINGCUSTOM_H

#include "medGuiExport.h"
#include "medToolBox.h"

class medToolBoxFiltering;
class medFilteringAbstractToolBoxPrivate;
class dtkAbstractData;

class MEDGUI_EXPORT medFilteringAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
             medFilteringAbstractToolBox(QWidget *parentToolBox = 0);
    virtual ~medFilteringAbstractToolBox(void);

    void setFilteringToolBox(medToolBoxFiltering *toolbox);

    virtual dtkAbstractData* processOutput() = 0;

protected:
    medToolBoxFiltering *parentToolBox(void);

private:
    medFilteringAbstractToolBoxPrivate *d;
};

#endif
