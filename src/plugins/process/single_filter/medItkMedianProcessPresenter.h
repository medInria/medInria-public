/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMedianFilterProcessPresenter.h>
#include <medItkMedianProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPLUGIN_EXPORTS medItkMedianProcessPresenter: public medAbstractMedianFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkMedianProcessPresenter(medItkMedianProcess *parent)
        : medAbstractMedianFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkMedianProcess* process() const {return m_process;}
private:
    medItkMedianProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractMedianFilterProcess, medItkMedianProcess)
