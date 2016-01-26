/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMultiplyFilterProcessPresenter.h>
#include <medItkMultiplyProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPROCESSPLUGINS_EXPORTS medItkMultiplyProcessPresenter: public medAbstractMultiplyFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkMultiplyProcessPresenter(medItkMultiplyProcess *parent)
        : medAbstractMultiplyFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkMultiplyProcess* process() const {return m_process;}
private:
    medItkMultiplyProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractMultiplyFilterProcess, medItkMultiplyProcess)
