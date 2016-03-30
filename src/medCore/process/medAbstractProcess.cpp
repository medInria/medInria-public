/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>
#include <medMessageController.h>
#include <dtkCore/dtkAbstractObject.h>

medAbstractProcess::medAbstractProcess(medAbstractProcess * parent) : dtkAbstractProcess(*new medAbstractProcessPrivate(this), parent)
{
}

medAbstractProcess::medAbstractProcess(const medAbstractProcess& other) : dtkAbstractProcess(*new medAbstractProcessPrivate(*other.d_func()), other)
{
    
}

medAbstractProcess::~medAbstractProcess()
{
}

void medAbstractProcess::displayVolumeError(QString id)
{
    QString error;
    if ( !id.contains("3"))
    {
        error = "This toolbox is designed to be used with 3D volumes";
    }
    else
    {
        error = "Pixel type not yet implemented (" + id + ")";
    }
    qDebug() << description() + ": " + error;
    medMessageController::instance()->showError(error, 3000);
}

int medAbstractProcess::isMeshTypeError(QString id)
{
    if(!id.contains("vtkDataMesh"))
    {
        QString error = "This toolbox is designed to be used with meshes";
        qDebug() << description() + ": " + error;
        medMessageController::instance()->showError(error, 3000);

        return DTK_FAILURE;
    }
    return DTK_SUCCEED;
}
