#pragma once

#include <medAbstractTractographyProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>
#include <medDoubleParameter.h>

#include "ttkTensorTractographyProcessPluginExport.h"

class TTKTENSORTRACTOGRAPHYPROCESSPLUGIN_EXPORT ttkTensorTractographyProcess: public medAbstractTractographyProcess
{
    Q_OBJECT
public:
    ttkTensorTractographyProcess(QObject* parent = NULL);
    ~ttkTensorTractographyProcess();

    static void eventCallback(itk::Object *caller, const itk::EventObject& event, void *clientData)
    {
        medAbstractTractographyProcess * source = reinterpret_cast<medAbstractTractographyProcess *>(clientData);
        itk::ProcessObject * processObject = (itk::ProcessObject*) caller;
        source->progression()->setValue(processObject->GetProgress() * 100);
    }

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

    medIntParameter *faThreshold() const;
    medIntParameter *faThreshold2() const;
    medIntParameter *smoothness() const;
    medIntParameter *minLength() const;
    medIntParameter *sampling() const;

private:
    template <class inputType> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_logerfilter;
    itk::SmartPointer<itk::ProcessObject> m_tractographyfilter;
    itk::SmartPointer<itk::ProcessObject> m_converterfilter;

    medIntParameter *m_faThreshold;
    medIntParameter *m_faThreshold2;
    medIntParameter *m_smoothness;
    medIntParameter *m_minLength;
    medIntParameter *m_sampling;
};

inline medAbstractTractographyProcess* ttkTensorTractographyProcessCreator()
{
    return new ttkTensorTractographyProcess();
}
