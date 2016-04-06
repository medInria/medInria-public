/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractInvertFilterProcessPresenter.h>
#include <medItkInvertProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPROCESSPLUGINS_EXPORTS medItkInvertProcessPresenter: public medAbstractInvertFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkInvertProcessPresenter(medItkInvertProcess *parent)
        : medAbstractInvertFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkInvertProcess* process() const {return m_process;}
private:
    medItkInvertProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractInvertFilterProcess, medItkInvertProcess)
