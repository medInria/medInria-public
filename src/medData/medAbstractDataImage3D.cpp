/* dtkAbstractDataImage3D.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Feb 11 12:40:15 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Jul 31 23:17:38 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <dtkCore/dtkAbstractDataImage3D.h>

class dtkAbstractDataImage3DPrivate
{
public:
};

dtkAbstractDataImage3D::dtkAbstractDataImage3D(void) : dtkAbstractDataImage(), d(new dtkAbstractDataImage3DPrivate)
{

}

dtkAbstractDataImage3D::~dtkAbstractDataImage3D(void)
{
    delete d;

    d = NULL;
}

int dtkAbstractDataImage3D::Dimension() const {
    return 3;
}
