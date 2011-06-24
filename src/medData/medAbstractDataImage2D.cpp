/* dtkAbstractDataImage2D.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Feb 11 12:40:15 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Jul 31 23:16:34 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <dtkCore/dtkAbstractDataImage2D.h>

class dtkAbstractDataImage2DPrivate
{
public:
};

dtkAbstractDataImage2D::dtkAbstractDataImage2D(void) : dtkAbstractDataImage(), d(new dtkAbstractDataImage2DPrivate)
{

}

dtkAbstractDataImage2D::~dtkAbstractDataImage2D(void)
{
    delete d;

    d = NULL;
}

int dtkAbstractDataImage2D::Dimension() const {
    return 2;
}

