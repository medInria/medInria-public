/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSubtractFilterProcessPresenter.h>
#include <medItkSubtractProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPROCESSPLUGINS_EXPORTS medItkSubtractProcessPresenter: public medAbstractSubtractFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkSubtractProcessPresenter(medItkSubtractProcess *parent)
        : medAbstractSubtractFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkSubtractProcess* process() const {return m_process;}
private:
    medItkSubtractProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractSubtractFilterProcess, medItkSubtractProcess)
