/* medSegmentationAbstractToolBox.h ---
 *
 * Author: John Stark
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

#ifndef medSegmentationAbstractToolBox_H
#define medSegmentationAbstractToolBox_H

#include "medToolBox.h"
#include "medGuiExport.h"

class medSegmentationSelectorToolBox;
class medSegmentationAbstractToolBoxPrivate;

//! Base class for custom segmentation algoithms
class MEDGUI_EXPORT medSegmentationAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
    //! Parent should be a medSegmentationSelectorToolBox
             medSegmentationAbstractToolBox(QWidget *parent = 0);
    virtual ~medSegmentationAbstractToolBox();

protected:
    //! Get the segmentationToolbox (usually one instance)
    medSegmentationSelectorToolBox *segmentationToolBox();

private:
    medSegmentationAbstractToolBoxPrivate *d;
};

#endif
