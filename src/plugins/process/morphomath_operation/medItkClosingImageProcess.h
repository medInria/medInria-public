/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractClosingImageProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>

#include <medItkMorphomathOperationProcessPluginExport.h>

class medItkClosingImageProcessPrivate;

class MEDITKMORPHOMATHOPERATIONPROCESSPLUGIN_EXPORT medItkClosingImageProcess: public medAbstractClosingImageProcess
{
    Q_OBJECT
public:
    static void eventCallback(itk::Object *caller, const itk::EventObject& event, void *clientData)
    {
        medItkClosingImageProcess * source = reinterpret_cast<medItkClosingImageProcess *>(clientData);
        itk::ProcessObject * processObject = (itk::ProcessObject*) caller;
        source->progression()->setValue(processObject->GetProgress() * 100);
    }

    medItkClosingImageProcess(QObject* parent = nullptr);
    ~medItkClosingImageProcess();

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    template <class inputType> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractClosingImageProcess* medItkClosingImageProcessCreator()
{
    return new medItkClosingImageProcess();
}
