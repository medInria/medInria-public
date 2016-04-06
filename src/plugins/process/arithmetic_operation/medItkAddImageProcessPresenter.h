/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractAddImageProcessPresenter.h>
#include <medItkAddImageProcess.h>

#include <medItkArithmeticOperationProcessPluginExport.h>

class MEDITKARITHMETICOPERATIONPROCESSPLUGINS_EXPORT medItkAddImageProcessPresenter: public medAbstractAddImageProcessPresenter
{
    Q_OBJECT
public:
    medItkAddImageProcessPresenter(medItkAddImageProcess *parent)
        : medAbstractAddImageProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkAddImageProcess* process() const {return m_process;}
private:
    medItkAddImageProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractAddImageProcess, medItkAddImageProcess)
