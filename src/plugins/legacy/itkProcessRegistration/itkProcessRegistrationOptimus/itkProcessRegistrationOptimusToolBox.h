/* itkProcessRegistrationOptimusToolBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 15:39:43 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 16:28:26 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 22
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#pragma once

#include <medRegistrationAbstractToolBox.h>

class itkProcessRegistrationOptimusToolBoxPrivate;

class itkProcessRegistrationOptimusToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Optimus",
                          "interface to the optimus algorithm",
                          <<"registration")

public:
     itkProcessRegistrationOptimusToolBox(QWidget *parentToolBox = 0);
    ~itkProcessRegistrationOptimusToolBox(void);

public:
    static bool registered(void);

public slots:
    void run(void);

private:
    itkProcessRegistrationOptimusToolBoxPrivate *d;
};
