/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDivideImageProcess.h>

class medItkDivideImageProcessPrivate;

class medItkDivideImageProcess: public medAbstractDivideImageProcess
{
public:
    medItkDivideImageProcess(QObject* parent = NULL);
    ~medItkDivideImageProcess();

    virtual void run();
    virtual void cancel();

    virtual QString name() const {return "ITK divide image filter";}
    virtual QString description() const {return "A filter using the ITK library to perform the division between tow images";}
    virtual QString version() const {return "0.0.1";}

private:
    template <class inputType> void  _run();

private:
    medItkDivideImageProcessPrivate *d;
};

inline medAbstractDivideImageProcess* medItkDivideImageProcessCreator(void)
{
    return new medItkDivideImageProcess();
}
