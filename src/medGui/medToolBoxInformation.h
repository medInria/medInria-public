/* medToolBoxInformation.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:19:01 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 16:13:55 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 12
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXINFORMATION_H
#define MEDTOOLBOXINFORMATION_H

#include "medToolBox.h"

class medToolBoxInformationPrivate;

class MEDGUI_EXPORT medToolBoxInformation : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxInformation(QWidget *parent = 0);
    ~medToolBoxInformation(void);

private:
    medToolBoxInformationPrivate *d;
};

#endif
