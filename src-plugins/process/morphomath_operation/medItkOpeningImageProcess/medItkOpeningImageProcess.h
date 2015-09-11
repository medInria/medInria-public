/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractOpeningImageProcess.h>

class medItkOpeningImageProcessPrivate;

class MEDINRIA_EXPORT medItkOpeningImageProcess: public medAbstractOpeningImageProcess
{
public:
    medItkOpeningImageProcess(QObject* parent = NULL);
    ~medItkOpeningImageProcess();

    virtual void run();
    virtual void cancel();

    virtual medProcessDetails details() const;

private:
    template <class inputType> void  _run();

private:
    medItkOpeningImageProcessPrivate *d;
};

inline medAbstractOpeningImageProcess* medItkOpeningImageProcessCreator(void)
{
    return new medItkOpeningImageProcess();
}
