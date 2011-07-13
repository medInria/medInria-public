/* medAbstractDataImage3D.cpp --- 
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

#include <medAbstractDataImage3D.h>

class medAbstractDataImage3DPrivate
{
public:
};

medAbstractDataImage3D::medAbstractDataImage3D(void) : medAbstractDataImage(), d(new medAbstractDataImage3DPrivate)
{

}

medAbstractDataImage3D::~medAbstractDataImage3D(void)
{
    delete d;

    d = NULL;
}

int medAbstractDataImage3D::Dimension() const {
    return 3;
}
