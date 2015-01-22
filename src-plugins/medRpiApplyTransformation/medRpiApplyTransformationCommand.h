/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractApplyTransformationCommand.h>

#include <itkProcessRegistration.h>
#include <itkImageRegistrationFactory.h>

class medRpiApplyTransformationCommand : public QObject, public medAbstractApplyTransformationCommand
{
    Q_OBJECT

public:
    typedef itk::Image< float, 3 > RegImageType;

    medRpiApplyTransformationCommand(medAbstractTransformation *transfo, itk::ImageRegistrationFactory<RegImageType>::Pointer factory);
    ~medRpiApplyTransformationCommand();

    virtual void undo();
    virtual void redo();

    medAbstractData* output();

signals:
    void commandUndone();
    void commandDone();

private:
    medAbstractTransformation *m_transfo;
    itk::ImageRegistrationFactory<RegImageType>::Pointer m_factory;
    medAbstractData* m_output;
};
