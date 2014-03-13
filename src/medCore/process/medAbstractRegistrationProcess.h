/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <medAbstractProcess.h>

class medAbstractData;

class MEDCORE_EXPORT medAbstractRegistrationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractRegistrationProcess(medAbstractProcess *parent = NULL);
    virtual ~medAbstractRegistrationProcess();

public:
    virtual void setFixedInput(medAbstractData *data) = 0;
    virtual void setMovingInput(medAbstractData *data) = 0;

};
