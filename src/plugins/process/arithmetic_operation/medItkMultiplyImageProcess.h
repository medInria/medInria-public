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

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medPluginExport.h>

class MEDPLUGINS_EXPORT medItkMultiplyImageProcess: public medAbstractMultiplyImageProcess
{
    Q_OBJECT
public:
    medItkMultiplyImageProcess(QObject* parent = NULL);
    ~medItkMultiplyImageProcess();

    virtual void run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    template<class inputType> void _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractMultiplyImageProcess* medItkMultiplyImageProcessCreator()
{
    return new medItkMultiplyImageProcess();
}
