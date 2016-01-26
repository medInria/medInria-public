/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDivideFilterProcessPresenter.h>
#include <medItkDivideProcess.h>

#include <medItkSingleFilterOperationProcessPluginExport.h>

class MEDITKSINGLEFILTEROPERATIONPROCESSPLUGINS_EXPORTS medItkDivideProcessPresenter: public medAbstractDivideFilterProcessPresenter
{
    Q_OBJECT
public:
    medItkDivideProcessPresenter(medItkDivideProcess *parent)
        : medAbstractDivideFilterProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkDivideProcess* process() const {return m_process;}
private:
    medItkDivideProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractDivideFilterProcess, medItkDivideProcess)
