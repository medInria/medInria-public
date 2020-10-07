/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMultiplyImageProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>
#include <itkImage.h>

#include <medIntParameter.h>

#include <medItkArithmeticOperationProcessPluginExport.h>

class MEDITKARITHMETICOPERATIONPROCESSPLUGINS_EXPORT medItkMultiplyImageProcess: public medAbstractMultiplyImageProcess
{
    Q_OBJECT
public:
    static void eventCallback(itk::Object *caller, const itk::EventObject& event, void *clientData)
    {
        medItkMultiplyImageProcess * source = reinterpret_cast<medItkMultiplyImageProcess *>(clientData);
        itk::ProcessObject * processObject = (itk::ProcessObject*) caller;
        source->progression()->setValue(processObject->GetProgress() * 100);
    }

    medItkMultiplyImageProcess(QObject* parent = nullptr);
    ~medItkMultiplyImageProcess();

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    typedef itk::Image<double, 3> ImageType;
    ImageType::Pointer CastImage(medAbstractImageData *data);
    template <class inputType> ImageType::Pointer _cast(medAbstractImageData *data);

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractMultiplyImageProcess* medItkMultiplyImageProcessCreator()
{
    return new medItkMultiplyImageProcess();
}
