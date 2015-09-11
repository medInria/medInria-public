/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractClosingImageProcess.h>

class medItkClosingImageProcessPrivate;

class MEDINRIA_EXPORT medItkClosingImageProcess: public medAbstractClosingImageProcess
{
public:
    medItkClosingImageProcess(QObject* parent = NULL);
    ~medItkClosingImageProcess();

    virtual void run();
    virtual void cancel();

    virtual medProcessDetails details() const;

private:
    template <class inputType> void  _run();

private:
    medItkClosingImageProcessPrivate *d;
};

inline medAbstractClosingImageProcess* medItkClosingImageProcessCreator(void)
{
    return new medItkClosingImageProcess();
}
