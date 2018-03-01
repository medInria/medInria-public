/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMultiplyImageProcessPresenter.h>
#include <medItkMultiplyImageProcess.h>

#include <medItkArithmeticOperationProcessPluginExport.h>

class MEDITKARITHMETICOPERATIONPROCESSPLUGINS_EXPORT medItkMultiplyImageProcessPresenter: public medAbstractMultiplyImageProcessPresenter
{
    Q_OBJECT
public:
    medItkMultiplyImageProcessPresenter(medItkMultiplyImageProcess *parent)
        : medAbstractMultiplyImageProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkMultiplyImageProcess* process() const {return m_process;}
private:
    medItkMultiplyImageProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractMultiplyImageProcess, medItkMultiplyImageProcess)
