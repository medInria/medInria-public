/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDWIMaskingProcess.h>

#include <itkProcessObject.h>
#include <itkSmartPointer.h>

#include <medIntParameter.h>

#include <medItkDWIBasicThresholdingProcessPluginExport.h>

class MEDITKDWIBASICTHRESHOLDINGPROCESSPLUGIN_EXPORT medItkDWIBasicThresholdingProcess: public medAbstractDWIMaskingProcess
{
    Q_OBJECT
public:
    medItkDWIBasicThresholdingProcess(QObject* parent = nullptr);
    ~medItkDWIBasicThresholdingProcess();

    virtual void setInput(medAbstractImageData* data);

    virtual medAbstractJob::medJobExitStatus run();
    virtual void cancel();

    virtual QString caption() const;
    virtual QString description() const;

    medIntParameter *b0VolumeNumberParameter() const;
    medIntParameter *thresholdValueParameter() const;

private:
    template<class inputType> medAbstractJob::medJobExitStatus _run();

private:
    itk::SmartPointer<itk::ProcessObject> m_extractfilter;
    itk::SmartPointer<itk::ProcessObject> m_thresholdfilter;

    medIntParameter *m_b0VolumeNumber;
    medIntParameter *m_thresholdValue;
};

inline medAbstractDWIMaskingProcess* medItkDWIBasicThresholdingProcessCreator()
{
    return new medItkDWIBasicThresholdingProcess();
}
