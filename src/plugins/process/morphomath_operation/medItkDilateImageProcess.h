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

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medPluginExport.h>

class medItkDilateImageProcessPrivate;

class MEDPLUGINS_EXPORT medItkDilateImageProcess: public medAbstractDilateImageProcess
{
    Q_OBJECT
public:
    medItkDilateImageProcess(QObject* parent = NULL);
    ~medItkDilateImageProcess();

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    template <class inputType> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractDilateImageProcess* medItkDilateImageProcessCreator()
{
    return new medItkDilateImageProcess();
}
