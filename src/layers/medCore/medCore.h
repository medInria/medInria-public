/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>

#include <medAbstractArithmeticOperationProcess.h>
#include <medAbstractAddImageProcess.h>
#include <medAbstractSubtractImageProcess.h>
#include <medAbstractMultiplyImageProcess.h>
#include <medAbstractDivideImageProcess.h>

#include <medAbstractMorphomathOperationProcess.h>
#include <medAbstractErodeImageProcess.h>
#include <medAbstractDilateImageProcess.h>
#include <medAbstractOpeningImageProcess.h>
#include <medAbstractClosingImageProcess.h>

#include <medCoreExport.h>


namespace medCore
{
    namespace pluginManager
    {
        MEDCORE_EXPORT void initialize(const QString& path = QString(), bool verbose = true);
    }

    namespace arithmeticalOperation
    {
        MEDCORE_EXPORT medAbstractArithmeticOperationProcessPluginManager& pluginManager(void);
        MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);

        namespace addImage
        {
            MEDCORE_EXPORT medAbstractAddImageProcessPluginFactory& pluginFactory(void);
        }
        namespace subtractImage
        {
            MEDCORE_EXPORT medAbstractSubtractImageProcessPluginFactory& pluginFactory(void);
        }
        namespace multiplyImage
        {
            MEDCORE_EXPORT medAbstractMultiplyImageProcessPluginFactory& pluginFactory(void);
        }
        namespace divideImage
        {
            MEDCORE_EXPORT medAbstractDivideImageProcessPluginFactory& pluginFactory(void);
        }
    }

    namespace morphomathOperation
    {
        MEDCORE_EXPORT medAbstractMorphomathOperationProcessPluginManager& pluginManager(void);
        MEDCORE_EXPORT void initialize(const QString& path, bool verbose = true);

        namespace erodeImage
        {
            MEDCORE_EXPORT medAbstractErodeImageProcessPluginFactory& pluginFactory(void);   
        }
        namespace dilateImage
        {
            MEDCORE_EXPORT medAbstractDilateImageProcessPluginFactory& pluginFactory(void);
        }
        namespace openingImage
        {
            MEDCORE_EXPORT medAbstractOpeningImageProcessPluginFactory& pluginFactory(void);
        }
        namespace closingImage
        {
            MEDCORE_EXPORT medAbstractClosingImageProcessPluginFactory& pluginFactory(void);
        }
    }
}
