/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMaskImageProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>

#include <medItkMaskImageProcessPluginExport.h>

class medItkMaskImageProcessPrivate;

class MEDITKMASKIMAGEPROCESSPLUGIN_EXPORT medItkMaskImageProcess: public medAbstractMaskImageProcess
{
    Q_OBJECT
public:
    static void eventCallback(itk::Object *caller, const itk::EventObject& event, void *clientData)
    {
        medAbstractMaskImageProcess * source = reinterpret_cast<medAbstractMaskImageProcess *>(clientData);
        itk::ProcessObject * processObject = (itk::ProcessObject*) caller;
        source->progression()->setValue(processObject->GetProgress() * 100);
    }


    medItkMaskImageProcess(QObject* parent = NULL);
    ~medItkMaskImageProcess();

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

private:
    template <class inputType, unsigned int Dimension> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;
};

inline medAbstractMaskImageProcess* medItkMaskImageProcessCreator()
{
    return new medItkMaskImageProcess();
}
