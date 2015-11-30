/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medCore.h>

namespace medCore
{

namespace pluginManager
{
    void initialize(const QString& path, bool verbose)
    {
        for(QString const& realpath : path.split(';'))
        {
            if(realpath.isEmpty())
                break;
            medCore::arithmeticalOperation::initialize(realpath, verbose);
            medCore::morphomathOperation::initialize(realpath, verbose);
        }
    }
}

// artithmetic
namespace arithmeticalOperation
{
    namespace _private
    {
        medAbstractArithmeticOperationProcessPluginManager manager;
    }

    medAbstractArithmeticOperationProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }
    void initialize(const QString& path, bool verbose)
    {
        if(verbose)
            pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }

    namespace addImage
    {
        namespace _private
        {
            medAbstractAddImageProcessPluginFactory factory;
        }

        medAbstractAddImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace subtractImage
    {
        namespace _private
        {
            medAbstractSubtractImageProcessPluginFactory factory;
        }
        medAbstractSubtractImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace multiplyImage
    {
        namespace _private
        {
            medAbstractMultiplyImageProcessPluginFactory factory;
        }
        medAbstractMultiplyImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace divideImage
    {
        namespace _private
        {
            medAbstractDivideImageProcessPluginFactory factory;
        }

        medAbstractDivideImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
}

// morphomath
namespace morphomathOperation
{

    namespace _private
    {
        medAbstractMorphomathOperationProcessPluginManager manager;
    }

    medAbstractMorphomathOperationProcessPluginManager& pluginManager(void)
    {
        return _private::manager;
    }
    void initialize(const QString& path, bool verbose)
    {
        if(verbose)
            pluginManager().setVerboseLoading(verbose);
        pluginManager().initialize(path);
    }

    namespace erodeImage
    {
        namespace _private
        {
            medAbstractErodeImageProcessPluginFactory factory;
        }
        medAbstractErodeImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace dilateImage
    {
        namespace _private
        {
            medAbstractDilateImageProcessPluginFactory factory;
        }

        medAbstractDilateImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace openingImage
    {
        namespace _private
        {
            medAbstractOpeningImageProcessPluginFactory factory;
        }

        medAbstractOpeningImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
    namespace closingImage
    {
        namespace _private
        {
            medAbstractClosingImageProcessPluginFactory factory;
        }
        medAbstractClosingImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }
    }
}


}// end of medProcessLayer

