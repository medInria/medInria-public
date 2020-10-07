/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractDilateImageProcessPresenter.h>
#include <medItkDilateImageProcess.h>

#include <medItkMorphomathOperationProcessPluginExport.h>

class MEDITKMORPHOMATHOPERATIONPROCESSPLUGIN_EXPORT medItkDilateImageProcessPresenter: public medAbstractDilateImageProcessPresenter
{
    Q_OBJECT
public:
    medItkDilateImageProcessPresenter(medItkDilateImageProcess *parent)
        : medAbstractDilateImageProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkDilateImageProcess* process() const {return m_process;}
private:
    medItkDilateImageProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractDilateImageProcess, medItkDilateImageProcess)
