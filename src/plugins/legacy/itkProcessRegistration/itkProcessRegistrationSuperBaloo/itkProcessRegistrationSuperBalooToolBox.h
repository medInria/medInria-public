/* itkProcessRegistrationSuperBalooToolBox.h ---
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

class itkProcessRegistrationSuperBalooToolBoxPrivate;

class itkProcessRegistrationSuperBalooToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Super Baloo",
                          "interface to the Super Baloo algorithm",
                          <<"registration")

public:
     itkProcessRegistrationSuperBalooToolBox(QWidget *parentToolBox = 0);
    ~itkProcessRegistrationSuperBalooToolBox(void);

public:
    static bool registered(void);

public slots:
    void run(void);

private slots:
    void onAdvancedBoxToggled(bool toggle);

private:
    itkProcessRegistrationSuperBalooToolBoxPrivate *d;
};
