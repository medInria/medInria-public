/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractWindowingFilterProcessPresenter.h>
#include <medItkWindowingProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPROCESSPLUGINS_EXPORTS medItkWindowingProcessPresenter: public medAbstractWindowingFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkWindowingProcessPresenter(medItkWindowingProcess *parent)
        : medAbstractWindowingFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkWindowingProcess* process() const {return m_process;}
private:
    medItkWindowingProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractWindowingFilterProcess, medItkWindowingProcess)
