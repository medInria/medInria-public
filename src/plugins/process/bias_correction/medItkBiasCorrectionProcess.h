/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2017. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractBiasCorrectionProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>

#include <medItkBiasCorrectionProcessPluginExport.h>

class medItkBiasCorrectionProcessPrivate;

class MEDITKBIASCORRECTIONPROCESSPLUGIN_EXPORT medItkBiasCorrectionProcess: public medAbstractBiasCorrectionProcess
{
    Q_OBJECT
public:
    static void eventCallback(itk::Object *caller, const itk::EventObject& event, void *clientData)
    {
        medAbstractBiasCorrectionProcess * source = reinterpret_cast<medAbstractBiasCorrectionProcess *>(clientData);
        itk::ProcessObject * processObject = (itk::ProcessObject*) caller;
        source->progression()->setValue(processObject->GetProgress() * 100);
    }


    medItkBiasCorrectionProcess(QObject* parent = NULL);
    ~medItkBiasCorrectionProcess();

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    template <class inputType, unsigned int Dimension> medAbstractJob::medJobExitStatus _run();
    void N4BiasCorrectionCore();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractBiasCorrectionProcess* medItkBiasCorrectionProcessCreator()
{
    return new medItkBiasCorrectionProcess();
}
