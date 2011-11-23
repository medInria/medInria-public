/* medToolBoxSegmentationCustom.h ---
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

#ifndef MEDTOOLBOXSEGMENTATIONCUSTOM_H
#define MEDTOOLBOXSEGMENTATIONCUSTOM_H

#include "medToolBox.h"
#include "medGuiExport.h"

class medToolBoxSegmentation;
class medToolBoxSegmentationCustomPrivate;

//! Base class for custom segmentation algoithms
class MEDGUI_EXPORT medToolBoxSegmentationCustom : public medToolBox
{
    Q_OBJECT

public:
    //! Parent should be a medToolBoxSegmentation
             medToolBoxSegmentationCustom(QWidget *parent = 0);
    virtual ~medToolBoxSegmentationCustom(void);

protected:
    //! Get the segmentationToolbox (usually one instance)
    medToolBoxSegmentation *segmentationToolBox(void);

private:
    medToolBoxSegmentationCustomPrivate *d;
};

#endif
