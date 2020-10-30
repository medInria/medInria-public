#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSelectableToolBox.h>

class itkProcessRegistrationOptimusToolBoxPrivate;

class itkProcessRegistrationOptimusToolBox : public medAbstractSelectableToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Optimus", "interface to the optimus algorithm", <<"Registration")

public:
     itkProcessRegistrationOptimusToolBox(QWidget *parentToolBox = 0);
    ~itkProcessRegistrationOptimusToolBox(void);

    dtkPlugin * plugin() override;
    static bool registered(void);
    medAbstractData * processOutput() override;

public slots:
    void run(void);

private:
    itkProcessRegistrationOptimusToolBoxPrivate *d;
};
