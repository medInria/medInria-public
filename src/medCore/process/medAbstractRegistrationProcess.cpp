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
    medProcessInput<medAbstractImageData*> *fixed = new medProcessInput<medAbstractImageData*>("Fixed Image", false);
    this->appendInput( fixed );

    medProcessInput<medAbstractImageData*> *moving = new medProcessInput<medAbstractImageData*>("Moving Image", false);
    this->appendInput( moving );

    this->appendOutput( new medProcessOutput<medAbstractImageData*>("Output"));

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
