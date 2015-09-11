/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractAddImageProcess.h>

class medItkAddImageProcessPrivate;

class MEDINRIA_EXPORT medItkAddImageProcess: public medAbstractAddImageProcess
{
public:
    medItkAddImageProcess(QObject* parent = NULL);
    ~medItkAddImageProcess();

    virtual void run();
    virtual void cancel();

    virtual medProcessDetails details() const;

private:
    template <class inputType> void  _run();

private:
    medItkAddImageProcessPrivate *d;
};

inline medAbstractAddImageProcess* medItkAddImageProcessCreator(void)
{
    return new medItkAddImageProcess();
}
