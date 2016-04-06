/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractSubtractImageProcessPresenter.h>
#include <medItkSubtractImageProcess.h>

#include <medItkArithmeticOperationProcessPluginExport.h>

class MEDITKARITHMETICOPERATIONPROCESSPLUGINS_EXPORT medItkSubtractImageProcessPresenter: public medAbstractSubtractImageProcessPresenter
{
    Q_OBJECT
public:
    medItkSubtractImageProcessPresenter(medItkSubtractImageProcess *parent)
        : medAbstractSubtractImageProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkSubtractImageProcess* process() const {return m_process;}
private:
    medItkSubtractImageProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractSubtractImageProcess, medItkSubtractImageProcess)
