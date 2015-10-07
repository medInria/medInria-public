/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractErodeImageProcess.h>

#include <medPluginExport.h>

class medItkErodeImageProcessPrivate;

class MEDPLUGIN_EXPORT medItkErodeImageProcess: public medAbstractErodeImageProcess
{
public:
    medItkErodeImageProcess(QObject* parent = NULL);
    ~medItkErodeImageProcess();

    virtual void run();
    virtual void cancel();

    virtual medProcessDetails details() const;

private:
    template <class inputType> void  _run();

private:
    medItkErodeImageProcessPrivate *d;
};

inline medAbstractErodeImageProcess* medItkErodeImageProcessCreator(void)
{
    return new medItkErodeImageProcess();
}
