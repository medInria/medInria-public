#pragma once

#include <medAbstractDWIMaskingProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>

#include <itkDWIBrainMaskCalculatorProcessPluginExport.h>

class ITKDWIBRAINMASKCALCULATORPROCESSPLUGIN_EXPORT itkDWIBrainMaskCalculatorProcess: public medAbstractDWIMaskingProcess
{
    Q_OBJECT
public:
    itkDWIBrainMaskCalculatorProcess(QObject* parent = NULL);
    ~itkDWIBrainMaskCalculatorProcess();

    virtual void setInput(medAbstractImageData *data);

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

    medIntParameter *offset() const;

private:
    template <class inputType> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_filter;

    medIntParameter *m_offset;
};

inline medAbstractDWIMaskingProcess* itkDWIBrainMaskCalculatorProcessCreator()
{
    return new itkDWIBrainMaskCalculatorProcess();
}
