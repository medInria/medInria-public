/* itkProcessRegistrationBalooToolBox.h ---
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

class itkProcessRegistrationBalooToolBoxPrivate;

class itkProcessRegistrationBalooToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Baloo",
                          "interface to the Baloo algorithm",
                          <<"registration")

public:
     itkProcessRegistrationBalooToolBox(QWidget *parentToolBox = 0);
    ~itkProcessRegistrationBalooToolBox(void);

public:
    static bool registered(void);

public slots:
    void run(void);

private slots:
    void onAdvancedBoxToggled(bool toggle);

private:
    itkProcessRegistrationBalooToolBoxPrivate *d;
};
