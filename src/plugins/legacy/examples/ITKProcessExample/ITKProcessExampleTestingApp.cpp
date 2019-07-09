/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractData.h>
#include <dtkCoreSupport/dtkAbstractDataFactory.h>
#include <dtkCoreSupport/dtkAbstractProcess.h>
#include <dtkCoreSupport/dtkAbstractProcessFactory.h>
#include <dtkCoreSupport/dtkGlobal.h>
#include <dtkCoreSupport/dtkPluginManager.h>

#include <QtCore>
#include <QtGui>
#include <QtDebug>

int main(int argc, char **argv)
{
    QCoreApplication application(argc, argv);
    application.setApplicationName("ITKProcessExampleTesting");
    application.setApplicationVersion("0.0.1");
    application.setOrganizationName("INRIA");
    application.setOrganizationDomain("FR");

    dtkPluginManager::instance()->initialize();

    dtkAbstractData *inputImage = dynamic_cast <dtkAbstractData *>(dtkAbstractDataFactory::instance()->create("itkDataImageFloat3"));
    inputImage->enableReader("itkNrrdDataImageReader");
    inputImage->read(argv[1]);

    dtkAbstractProcess *worker = dynamic_cast <dtkAbstractProcess *>(dtkAbstractProcessFactory::instance()->create("ITKProcessExampleGaussianBlur"));

    worker->setInput(inputImage);
    worker->setParameter(2.0,0);

    qDebug() << "in main, calling worker->update...";
    worker->update();

    worker->output()->enableWriter("itkNrrdDataImageWriter");
    worker->output()->write(argv[2]);

    qDebug() << "in main, after writing result image";

    dtkPluginManager::instance()->uninitialize();

    return DTK_SUCCEED;
}
