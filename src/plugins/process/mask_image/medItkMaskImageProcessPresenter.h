/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractMaskImageProcessPresenter.h>
#include <medItkMaskImageProcess.h>
#include <medProcessPresenterFactory.h>

#include <medItkMaskImageProcessPluginExport.h>

class MEDITKMASKIMAGEPROCESSPLUGIN_EXPORT medItkMaskImageProcessPresenter: public medAbstractMaskImageProcessPresenter
{
    Q_OBJECT
public:
    medItkMaskImageProcessPresenter(medItkMaskImageProcess *parent)
        : medAbstractMaskImageProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkMaskImageProcess* process() const {return m_process;}
private:
    medItkMaskImageProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractMaskImageProcess, medItkMaskImageProcess)
