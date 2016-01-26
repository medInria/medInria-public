/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractShrinkFilterProcessPresenter.h>
#include <medItkShrinkProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPROCESSPLUGINS_EXPORTS medItkShrinkProcessPresenter: public medAbstractShrinkFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkShrinkProcessPresenter(medItkShrinkProcess *parent)
        : medAbstractShrinkFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkShrinkProcess* process() const {return m_process;}
private:
    medItkShrinkProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractShrinkFilterProcess, medItkShrinkProcess)
