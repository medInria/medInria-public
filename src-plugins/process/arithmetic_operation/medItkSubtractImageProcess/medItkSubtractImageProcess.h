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

#include <medPluginExport.h>

class medItkSubtractImageProcessPrivate;

class MEDPLUGIN_EXPORT medItkSubtractImageProcess: public medAbstractSubtractImageProcess
{
public:
    medItkSubtractImageProcess(QObject* parent = NULL);
    ~medItkSubtractImageProcess();

    virtual void run();
    virtual void cancel();

    virtual medProcessDetails details() const;

private:
    template <class inputType> void  _run();

private:
    medItkSubtractImageProcessPrivate *d;
};

inline medAbstractSubtractImageProcess* medItkSubtractImageProcessCreator(void)
{
    return new medItkSubtractImageProcess();
}
