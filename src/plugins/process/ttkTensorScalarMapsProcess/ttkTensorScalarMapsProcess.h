#pragma once

#include <medAbstractDiffusionScalarMapsProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>
#include <medDoubleParameter.h>

#include <ttkTensorScalarMapsProcessPluginExport.h>

class TTKTENSORSCALARMAPSPROCESSPLUGIN_EXPORT ttkTensorScalarMapsProcess: public medAbstractDiffusionScalarMapsProcess
{
    Q_OBJECT
public:
    ttkTensorScalarMapsProcess(QObject* parent = NULL);
    ~ttkTensorScalarMapsProcess();

    static void eventCallback(itk::Object *caller, const itk::EventObject& event, void *clientData)
    {
        medAbstractDiffusionScalarMapsProcess * source = reinterpret_cast<medAbstractDiffusionScalarMapsProcess *>(clientData);
        itk::ProcessObject * processObject = (itk::ProcessObject*) caller;
        source->progression()->setValue(processObject->GetProgress() * 100);
    }

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

public slots:
    void selectRequestedScalarMap(QString mapRequested);

private:
    template <class inputType> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;

    QString m_scalarMapRequested;
};

inline medAbstractDiffusionScalarMapsProcess* ttkTensorScalarMapsProcessCreator()
{
    return new ttkTensorScalarMapsProcess();
}
