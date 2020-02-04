/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractAddFilterProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>
#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPLUGIN_EXPORTS medItkAddProcess: public medAbstractAddFilterProcess
{
    Q_OBJECT
public:
    static void eventCallback(itk::Object *caller, const itk::EventObject& event, void *clientData)
    {
        medItkAddProcess * source = reinterpret_cast<medItkAddProcess *>(clientData);
        itk::ProcessObject * processObject = (itk::ProcessObject*) caller;
        source->progression()->setValue(processObject->GetProgress() * 100);
    }

    medItkAddProcess(QObject* parent = nullptr);
    ~medItkAddProcess();

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    template<class inputType> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractAddFilterProcess* medItkAddProcessCreator()
{
    return new medItkAddProcess();
}
