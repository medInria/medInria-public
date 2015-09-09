/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMultiplyImageProcess.h>

class medItkMultiplyImageProcessPrivate;

class medItkMultiplyImageProcess: public medAbstractMultiplyImageProcess
{
public:
    medItkMultiplyImageProcess(QObject* parent = NULL);
    ~medItkMultiplyImageProcess();

    virtual void run();
    virtual void cancel();

    virtual medProcessDetails details() const;
private:
    template <class inputType> void  _run();

private:
    medItkMultiplyImageProcessPrivate *d;
};

inline medAbstractMultiplyImageProcess* medItkMultiplyImageProcessCreator(void)
{
    return new medItkMultiplyImageProcess();
}
