#pragma once

#include <medAbstractDiffusionModelEstimationProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medBoolParameterGroup.h>
#include <medBoolParameter.h>

#include <itkTensorEstimationProcessPluginExport.h>

class ITKTENSORESTIMATIONPROCESSPLUGIN_EXPORT itkTensorEstimationProcess: public medAbstractDiffusionModelEstimationProcess
{
    Q_OBJECT
public:
    itkTensorEstimationProcess(QObject* parent = NULL);
    ~itkTensorEstimationProcess();

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

    medBoolParameter *noSmoothing() const;
    medBoolParameter *lowSmoothing() const;
    medBoolParameter *mediumSmoothing() const;
    medBoolParameter *highSmoothing() const;

private:
    template <class inputType> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_estimationfilter;
    itk::SmartPointer<itk::ProcessObject> m_smoothingfilter;

    medBoolParameterGroup *m_smoothing;
    medBoolParameter *m_NoSmoothing;
    medBoolParameter *m_LowSmoothing;
    medBoolParameter *m_MediumSmoothing;
    medBoolParameter *m_HighSmoothing;
};

inline medAbstractDiffusionModelEstimationProcess* itkTensorEstimationProcessCreator()
{
    return new itkTensorEstimationProcess();
}
