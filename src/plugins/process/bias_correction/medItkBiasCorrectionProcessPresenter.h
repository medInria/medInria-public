/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2017. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractBiasCorrectionProcessPresenter.h>
#include <medItkBiasCorrectionProcess.h>
#include <medProcessPresenterFactory.h>

#include <medItkBiasCorrectionProcessPluginExport.h>

class MEDITKBIASCORRECTIONPROCESSPLUGIN_EXPORT medItkBiasCorrectionProcessPresenter: public medAbstractBiasCorrectionProcessPresenter
{
    Q_OBJECT
public:
    medItkBiasCorrectionProcessPresenter(medItkBiasCorrectionProcess *parent) : medAbstractBiasCorrectionProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkBiasCorrectionProcess* process() const {return m_process;}
private:
    medItkBiasCorrectionProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractBiasCorrectionProcess, medItkBiasCorrectionProcess)
