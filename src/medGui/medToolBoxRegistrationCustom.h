/* medToolBoxRegistrationCustom.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 14:16:25 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 14:19:23 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXREGISTRATIONCUSTOM_H
#define MEDTOOLBOXREGISTRATIONCUSTOM_H

#include "medGuiExport.h"
#include "medToolBox.h"

class MEDGUI_EXPORT medToolBoxRegistrationCustom : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxRegistrationCustom(void);
    ~medToolBoxRegistrationCustom(void);
};

#endif
