/* medAbstractDataImage2D.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Feb 11 12:38:11 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Jul 31 23:15:56 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDABSTRACTDATAIMAGE2D_H
#define MEDABSTRACTDATAIMAGE2D_H

#include "medDataExport.h"
#include <medData/medAbstractDataImage.h>

class medAbstractDataImage2DPrivate;

class medAbstractDataImage2D : public medAbstractDataImage
{
    Q_OBJECT

public:

    medAbstractDataImage2D(void);
   ~medAbstractDataImage2D(void);

   virtual int Dimension() const;

private:
   medAbstractDataImage2DPrivate *d;
};

#endif
