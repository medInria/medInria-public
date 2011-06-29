/* medAbstractDataImage4D.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Feb 11 12:38:11 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Jul 31 23:18:21 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDABSTRACTDATAIMAGE4D_H
#define MEDABSTRACTDATAIMAGE4D_H

#include "medCoreExport.h"
#include <medCore/medAbstractDataImage.h>

class medAbstractDataImage4DPrivate;

class medAbstractDataImage4D : public medAbstractDataImage
{
    Q_OBJECT

public:
    medAbstractDataImage4D(void);
   ~medAbstractDataImage4D(void);

    virtual int Dimension() const;

private:
   medAbstractDataImage4DPrivate *d;
};

#endif
