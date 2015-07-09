/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSubstractImageProcess.h>

class medItkSubstractImageProcessPrivate;

class medItkSubstractImageProcess: public medAbstractSubstractImageProcess
{
public:
    medItkSubstractImageProcess(QObject* parent = NULL);
    ~medItkSubstractImageProcess();

    virtual void start();

private:
    template <class inputType> void  _start();

private:
    medItkSubstractImageProcessPrivate *d;
};
