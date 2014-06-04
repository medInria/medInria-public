/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medRegistrationAbstractToolBox.h>

class itkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate;

class itkProcessRegistrationDiffeomorphicDemonsToolBox : public medRegistrationAbstractToolBox
{
    Q_OBJECT
    MED_TOOLBOX_INTERFACE("Diffeomorphic Demons",
                          "Diffeomorphic demons registration using the ITK implementation",
                          <<"registration")
public:
     itkProcessRegistrationDiffeomorphicDemonsToolBox(QWidget *parentToolBox = 0);
    ~itkProcessRegistrationDiffeomorphicDemonsToolBox();

public:
    static bool registered();

public slots:
    void run();

private:
    itkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate *d;
};



