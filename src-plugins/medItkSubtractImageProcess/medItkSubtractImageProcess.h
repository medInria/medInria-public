/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSubtractImageProcess.h>

class medItkSubtractImageProcessPrivate;

class medItkSubtractImageProcess: public medAbstractSubtractImageProcess
{
public:
    medItkSubtractImageProcess(QObject* parent = NULL);
    ~medItkSubtractImageProcess();

    virtual void run();
    virtual void cancel();

    // TODO use what is in the json file ? - RDE
    virtual QString name() const {return "ITK subtract image filter";}
    virtual QString description() const {return "A filter using the ITK library to perform the subtraction between tow images";}
    virtual QString version() const {return "0.0.1";}

private:
    template <class inputType> void  _run();

private:
    medItkSubtractImageProcessPrivate *d;
};

inline medAbstractSubtractImageProcess* medItkSubtractImageProcessCreator(void)
{
    return new medItkSubtractImageProcess();
}
