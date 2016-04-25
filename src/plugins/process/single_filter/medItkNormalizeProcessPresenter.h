/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractNormalizeFilterProcessPresenter.h>
#include <medItkNormalizeProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPLUGIN_EXPORTS medItkNormalizeProcessPresenter: public medAbstractNormalizeFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkNormalizeProcessPresenter(medItkNormalizeProcess *parent)
        : medAbstractNormalizeFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkNormalizeProcess* process() const {return m_process;}
private:
    medItkNormalizeProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractNormalizeFilterProcess, medItkNormalizeProcess)
