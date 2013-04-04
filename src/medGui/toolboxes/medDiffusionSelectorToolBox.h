/* medDiffusionSelectorToolBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:03:19 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#pragma once

#include "medToolBox.h"
#include "medGuiExport.h"

class dtkAbstractView;
class dtkAbstractData;
class dtkAbstractProcess;
class medDiffusionSelectorToolBoxPrivate;
class medDataIndex;

class MEDGUI_EXPORT medDiffusionSelectorToolBox : public medToolBox
{
    Q_OBJECT
public:
     medDiffusionSelectorToolBox(QWidget *parent = 0);
    ~medDiffusionSelectorToolBox();

    dtkAbstractData *output() const;

public slots:
    // void run();
    void clear();

    void onToolBoxChosen(int id);

private:
    medDiffusionSelectorToolBoxPrivate *d;

};


