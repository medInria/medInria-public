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

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medPluginExport.h>

class MEDPLUGINS_EXPORT medItkAddImageProcess: public medAbstractAddImageProcess
{
    Q_OBJECT
public:
    medItkAddImageProcess(QObject* parent = NULL);
    ~medItkAddImageProcess();

    virtual medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    template<class inputType> medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractAddImageProcess* medItkAddImageProcessCreator()
{
    return new medItkAddImageProcess();
}
