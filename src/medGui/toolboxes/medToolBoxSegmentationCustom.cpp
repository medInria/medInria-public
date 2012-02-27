/* medToolBoxSegmentationCustom.cpp ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 15:42:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:58:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxSegmentation.h"
#include "medToolBoxSegmentationCustom.h"

class medToolBoxSegmentationCustomPrivate
{
public:
    medToolBoxSegmentation * segmentationToolBox;
};

medToolBoxSegmentationCustom::medToolBoxSegmentationCustom(QWidget *parent) : medToolBox(parent), d(new medToolBoxSegmentationCustomPrivate)
{
    d->segmentationToolBox = qobject_cast<medToolBoxSegmentation*>(parent);
}

medToolBoxSegmentationCustom::~medToolBoxSegmentationCustom(void)
{
    delete d;
    d = NULL;
}

medToolBoxSegmentation *medToolBoxSegmentationCustom::segmentationToolBox(void)
{
    return d->segmentationToolBox;
}
