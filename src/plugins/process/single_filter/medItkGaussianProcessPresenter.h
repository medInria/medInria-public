/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractGaussianFilterProcessPresenter.h>
#include <medItkGaussianProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPLUGIN_EXPORTS medItkGaussianProcessPresenter: public medAbstractGaussianFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkGaussianProcessPresenter(medItkGaussianProcess *parent)
        : medAbstractGaussianFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkGaussianProcess* process() const {return m_process;}
private:
    medItkGaussianProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractGaussianFilterProcess, medItkGaussianProcess)
