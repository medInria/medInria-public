/* medAbstractDataImage.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Sat Oct  3 18:30:05 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sat Oct  3 18:57:18 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medAbstractDataImage.h"

// /////////////////////////////////////////////////////////////////
// medAbstractDataImagePrivate
// /////////////////////////////////////////////////////////////////

class medAbstractDataImagePrivate
{
public:
};

// /////////////////////////////////////////////////////////////////
// medAbstractDataImage
// /////////////////////////////////////////////////////////////////

medAbstractDataImage::medAbstractDataImage(void) : dtkAbstractDataImage(), d(new medAbstractDataImagePrivate)
{

}

medAbstractDataImage::medAbstractDataImage(const medAbstractDataImage& other): dtkAbstractDataImage(other), d(new medAbstractDataImagePrivate)
{

}

medAbstractDataImage::~medAbstractDataImage(void)
{
    delete d;

    d = NULL;
}
