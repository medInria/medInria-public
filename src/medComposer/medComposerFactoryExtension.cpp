/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposerFactoryExtension.h>

//include nodes

#include <medAddImageProcessNode.h>
#include <medSubtractImageProcessNode.h>
#include <medMultiplyImageProcessNode.h>
#include <medDivideImageProcessNode.h>

#include <medDilateImageProcessNode.h>
#include <medErodeImageProcessNode.h>
#include <medOpeningImageProcessNode.h>
#include <medClosingImageProcessNode.h>

medComposerFactoryExtension::medComposerFactoryExtension(void) : dtkComposerNodeFactoryExtension()
{

}

medComposerFactoryExtension::~medComposerFactoryExtension(void)
{

}

void medComposerFactoryExtension::extend(dtkComposerNodeFactory *factory)
{

//    QDir dir(":/");
//    qDebug() << "dir exist:" << dir.exists();
//    qDebug() << "content: " << dir.entryList(QStringList() << "");

    QString arithmetic_prefix = "/home/rdebroiz/medinria/src/src/medCore/process/arithmetic_operation/";
    QString morphomath_prefix = "/home/rdebroiz/medinria/src/src/medCore/process/morphomath_operation/";

    factory->record(arithmetic_prefix + "medAbstractAddImageProcess.json",
                    dtkComposerNodeCreator<medAddImageProcessNode>);
    factory->record(arithmetic_prefix + "medAbstractSubtractImageProcess.json",
                    dtkComposerNodeCreator<medSubtractImageProcessNode>);
    factory->record(arithmetic_prefix + "medAbstractMultiplyImageProcess.json",
                    dtkComposerNodeCreator<medMultiplyImageProcessNode>);
    factory->record(arithmetic_prefix + "medAbstractDivideImageProcess.json",
                    dtkComposerNodeCreator<medDivideImageProcessNode>);

    factory->record(morphomath_prefix + "medAbstractAddImageProcess.json",
                    dtkComposerNodeCreator<medErodeImageProcessNode>);
    factory->record(morphomath_prefix + "medAbstractDilateImageProcess.json",
                    dtkComposerNodeCreator<medDilateImageProcessNode>);
    factory->record(morphomath_prefix + "medAbstractOpeningImageProcess.json",
                    dtkComposerNodeCreator<medOpeningImageProcessNode>);
    factory->record(morphomath_prefix + "medAbstractClosingImageProcess.json",
                    dtkComposerNodeCreator<medClosingImageProcessNode>);

}
