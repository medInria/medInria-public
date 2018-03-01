/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDivideImageProcessPresenter.h>
#include <medItkDivideImageProcess.h>

#include <medItkArithmeticOperationProcessPluginExport.h>

class MEDITKARITHMETICOPERATIONPROCESSPLUGINS_EXPORT medItkDivideImageProcessPresenter: public medAbstractDivideImageProcessPresenter
{
    Q_OBJECT
public:
    medItkDivideImageProcessPresenter(medItkDivideImageProcess *parent)
        : medAbstractDivideImageProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkDivideImageProcess* process() const {return m_process;}
private:
    medItkDivideImageProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractDivideImageProcess, medItkDivideImageProcess)
