/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medItkSubstractImageProcess.h>

#include <dtkLog.h>

medItkSubstractImageProcess::medItkSubstractImageProcess(QObject *parent): medAbstractSubstractImageProcess(parent)
{

}

medItkSubstractImageProcess::~medItkSubstractImageProcess()
{

}

void medItkSubstractImageProcess::start()
{
    dtkDebug() << "start !!";
}
