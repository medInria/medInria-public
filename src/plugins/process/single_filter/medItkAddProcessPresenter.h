/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractAddFilterProcessPresenter.h>
#include <medItkAddProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPLUGIN_EXPORTS medItkAddProcessPresenter: public medAbstractAddFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkAddProcessPresenter(medItkAddProcess *parent)
        : medAbstractAddFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkAddProcess* process() const {return m_process;}
private:
    medItkAddProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractAddFilterProcess, medItkAddProcess)
