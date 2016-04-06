/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWindowingFilterProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPROCESSPLUGINS_EXPORTS medItkWindowingProcess: public medAbstractWindowingFilterProcess
{
    Q_OBJECT
public:
    static void eventCallback(itk::Object *caller, const itk::EventObject& event, void *clientData)
    {
        medItkWindowingProcess * source = reinterpret_cast<medItkWindowingProcess *>(clientData);
        itk::ProcessObject * processObject = (itk::ProcessObject*) caller;
        source->progression()->setValue(processObject->GetProgress() * 100);
    }

    medItkWindowingProcess(QObject* parent = NULL);
    ~medItkWindowingProcess();

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    template<class inputType> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractWindowingFilterProcess* medItkWindowingProcessCreator()
{
    return new medItkWindowingProcess();
}
