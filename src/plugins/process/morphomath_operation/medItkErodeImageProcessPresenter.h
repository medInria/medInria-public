/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractErodeImageProcessPresenter.h>
#include <medItkErodeImageProcess.h>

#include <medItkMorphomathOperationProcessPluginExport.h>

class MEDITKMORPHOMATHOPERATIONPROCESSPLUGIN_EXPORT medItkErodeImageProcessPresenter: public medAbstractErodeImageProcessPresenter
{
    Q_OBJECT
public:
    medItkErodeImageProcessPresenter(medItkErodeImageProcess *parent)
        : medAbstractErodeImageProcessPresenter(parent)
    {m_process = parent;}
    virtual medItkErodeImageProcess* process() const {return m_process;}
private:
    medItkErodeImageProcess *m_process;
};

MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractErodeImageProcess, medItkErodeImageProcess)
