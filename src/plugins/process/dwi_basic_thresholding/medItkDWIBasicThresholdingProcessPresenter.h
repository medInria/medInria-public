/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDWIMaskingProcessPresenter.h>
#include <medItkDWIBasicThresholdingProcess.h>
#include <medProcessPresenterFactory.h>

#include <medItkDWIBasicThresholdingProcessPluginExport.h>

class medIntParameterPresenter;

class MEDITKDWIBASICTHRESHOLDINGPROCESSPLUGIN_EXPORT medItkDWIBasicThresholdingProcessPresenter: public medAbstractDWIMaskingProcessPresenter
{
    Q_OBJECT
public:
    medItkDWIBasicThresholdingProcessPresenter(medItkDWIBasicThresholdingProcess *parent);

    virtual QWidget *buildToolBoxWidget();

    virtual medItkDWIBasicThresholdingProcess* process() const {return m_process;}

private:
    medItkDWIBasicThresholdingProcess *m_process;
    medIntParameterPresenter *m_b0VolPresenter;
    medIntParameterPresenter *m_thresholdPresenter;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractDWIMaskingProcess, medItkDWIBasicThresholdingProcess)
