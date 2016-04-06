/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractClosingImageProcessPresenter.h>
#include <medItkClosingImageProcess.h>

#include <medItkMorphomathOperationProcessPluginExport.h>

class MEDITKMORPHOMATHOPERATIONPROCESSPLUGIN_EXPORT medItkClosingImageProcessPresenter: public medAbstractClosingImageProcessPresenter
{
    Q_OBJECT
public:
    medItkClosingImageProcessPresenter(medItkClosingImageProcess *parent)
        : medAbstractClosingImageProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkClosingImageProcess* process() const {return m_process;}
private:
    medItkClosingImageProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractClosingImageProcess, medItkClosingImageProcess)
