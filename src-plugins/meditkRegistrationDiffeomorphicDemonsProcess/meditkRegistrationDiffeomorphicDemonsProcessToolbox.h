/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once
#include <medAbstractRegistrationProcess.h>
#include <medWidgets/medAbstractRegistrationToolBox.h>

class meditkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate;

class meditkProcessRegistrationDiffeomorphicDemonsToolBox : public medAbstractRegistrationToolbox
{
    Q_OBJECT

public:
     meditkProcessRegistrationDiffeomorphicDemonsToolBox(QWidget *parentToolBox = 0);
    ~meditkProcessRegistrationDiffeomorphicDemonsToolBox();
    
public:
    virtual void setProcess(medAbstractRegistrationProcess*);

private:
    meditkProcessRegistrationDiffeomorphicDemonsToolBoxPrivate *d;
};


