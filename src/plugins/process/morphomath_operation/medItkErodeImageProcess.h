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

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medPluginExport.h>

class medItkErodeImageProcessPrivate;

class MEDPLUGINS_EXPORT medItkErodeImageProcess: public medAbstractErodeImageProcess
{
    Q_OBJECT
public:
    medItkErodeImageProcess(QObject* parent = NULL);
    ~medItkErodeImageProcess();

    virtual void run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    template <class inputType> void  _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractErodeImageProcess* medItkErodeImageProcessCreator()
{
    return new medItkErodeImageProcess();
}
