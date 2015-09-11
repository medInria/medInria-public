/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDilateImageProcess.h>

class medItkDilateImageProcessPrivate;

class MEDINRIA_EXPORT medItkDilateImageProcess: public medAbstractDilateImageProcess
{
public:
    medItkDilateImageProcess(QObject* parent = NULL);
    ~medItkDilateImageProcess();

    virtual void run();
    virtual void cancel();

    virtual medProcessDetails details() const;

private:
    template <class inputType> void  _run();

private:
    medItkDilateImageProcessPrivate *d;
};

inline medAbstractDilateImageProcess* medItkDilateImageProcessCreator(void)
{
    return new medItkDilateImageProcess();
}
