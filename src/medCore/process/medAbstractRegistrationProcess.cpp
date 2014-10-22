/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractRegistrationProcess.h>

#include <medAbstractImageData.h>

medAbstractRegistrationProcess::medAbstractRegistrationProcess(medAbstractProcess *parent):
    medAbstractProcess(parent)
{
    medProcessInput<medAbstractImageData*> *fixed = new medProcessInput<medAbstractImageData*>;
    fixed->name = "Fixed Image";
    fixed->isOptional = false;
    this->appendInput( fixed );

    medProcessInput<medAbstractImageData*> *moving = new medProcessInput<medAbstractImageData*>;
    moving->name = "Moving Image";
    moving->isOptional = false;
    this->appendInput( moving );

    this->appendOutput( new medProcessInput<medAbstractImageData*>);

}

medAbstractRegistrationProcess::~medAbstractRegistrationProcess()
{

}

bool medAbstractRegistrationProcess::isInteractive()
{
    return false;
}

QList<medAbstractParameter*> medAbstractRegistrationProcess::parameters()
{
    //TODO GPR: temporary
    return QList<medAbstractParameter*>();
}
