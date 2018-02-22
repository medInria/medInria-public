/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractOpeningImageProcessPresenter.h>
#include <medItkOpeningImageProcess.h>

#include <medItkMorphomathOperationProcessPluginExport.h>

class MEDITKMORPHOMATHOPERATIONPROCESSPLUGIN_EXPORT medItkOpeningImageProcessPresenter: public medAbstractOpeningImageProcessPresenter
{
    Q_OBJECT
public:
    medItkOpeningImageProcessPresenter(medItkOpeningImageProcess *parent)
        : medAbstractOpeningImageProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkOpeningImageProcess* process() const {return m_process;}
private:
    medItkOpeningImageProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractOpeningImageProcess, medItkOpeningImageProcess)
