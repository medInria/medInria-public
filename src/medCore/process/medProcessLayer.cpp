/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medProcessLayer.h>

namespace medProcessLayer
{

medProcessDetails readDetailsFromJson(const QString &filePath)
{
    medProcessDetails details;

    QFile file(filePath);
    if(!file.open(QFile::ReadOnly))
    {
        dtkDebug() << Q_FUNC_INFO
                   << "Unable to read file" << filePath;
        return details;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    if (jsonDoc.isEmpty())
    {
        dtkDebug() << Q_FUNC_INFO
                   << "Json document" << filePath << "is empty. Unable to create json object.";
        return details;
    }

    QJsonObject json = jsonDoc.object();

    details.name = json.value(QString("name")).toString();
    details.version = json.value(QString("version")).toString();

    QVariantList dependencies = json.value(QString("dependencies")).toArray().toVariantList();
    for(const QVariant& dep : dependencies)
        details.dependences << dep.toString();

    file.close();

    return details;
}

namespace pluginManager
{
    void initialize(const QString& path, bool verbose)
    {
        QString realpath=path;
        if (path.isEmpty())
        {

            // Use settings to get plugins location.
//                dtkLinearAlgebraSparseSettings linear_algebra_sparse_settings;
//                linear_algebra_sparse_settings.beginGroup("linear-algebra-sparse");
//                realpath = linear_algebra_sparse_settings.value("plugins").toString();
//                linear_algebra_sparse_settings.endGroup();
        }
        medProcessLayer::arithmeticalOperation::addImage::initialize(realpath);
        medProcessLayer::arithmeticalOperation::subtractImage::initialize(realpath);
        medProcessLayer::arithmeticalOperation::multiplyImage::initialize(realpath);
        medProcessLayer::arithmeticalOperation::divideImage::initialize(realpath);

        medProcessLayer::morphomathOperation::erodeImage::initialize(realpath);
    }
}

// artithmetic
namespace arithmeticalOperation
{
    namespace addImage
    {
        namespace _private
        {
            medAbstractAddImageProcessPluginManager manager;
            medAbstractAddImageProcessPluginFactory factory;
        }

        medAbstractAddImageProcessPluginManager& pluginManager(void)
        {
            return _private::manager;
        }

        medAbstractAddImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }

        void initialize(const QString& path, bool verbose)
        {
            if(verbose)
                pluginManager().setVerboseLoading(verbose);
            pluginManager().initialize(path);
        }
    }
    namespace subtractImage
    {
        namespace _private
        {
            medAbstractSubtractImageProcessPluginManager manager;
            medAbstractSubtractImageProcessPluginFactory factory;
        }

        medAbstractSubtractImageProcessPluginManager& pluginManager(void)
        {
            return _private::manager;
        }

        medAbstractSubtractImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }

        void initialize(const QString& path, bool verbose)
        {
            if(verbose)
                pluginManager().setVerboseLoading(verbose);
            pluginManager().initialize(path);
        }
    }
    namespace multiplyImage
    {
        namespace _private
        {
            medAbstractMultiplyImageProcessPluginManager manager;
            medAbstractMultiplyImageProcessPluginFactory factory;
        }

        medAbstractMultiplyImageProcessPluginManager& pluginManager(void)
        {
            return _private::manager;
        }

        medAbstractMultiplyImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }

        void initialize(const QString& path, bool verbose)
        {
            if(verbose)
                pluginManager().setVerboseLoading(verbose);
            pluginManager().initialize(path);
        }
    }
    namespace divideImage
    {
        namespace _private
        {
            medAbstractDivideImageProcessPluginManager manager;
            medAbstractDivideImageProcessPluginFactory factory;
        }

        medAbstractDivideImageProcessPluginManager& pluginManager(void)
        {
            return _private::manager;
        }

        medAbstractDivideImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }

        void initialize(const QString& path, bool verbose)
        {
            if(verbose)
                pluginManager().setVerboseLoading(verbose);
            pluginManager().initialize(path);
        }
    }
}

// morphomath
namespace morphomathOperation
{
    namespace erodeImage
    {
        namespace _private
        {
            medAbstractErodeImageProcessPluginManager manager;
            medAbstractErodeImageProcessPluginFactory factory;
        }

        medAbstractErodeImageProcessPluginManager& pluginManager(void)
        {
            return _private::manager;
        }

        medAbstractErodeImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }

        void initialize(const QString& path, bool verbose)
        {
            if(verbose)
                pluginManager().setVerboseLoading(verbose);
            pluginManager().initialize(path);
        }
    }
    namespace dilateImage
    {
        namespace _private
        {
            medAbstractDilateImageProcessPluginManager manager;
            medAbstractDilateImageProcessPluginFactory factory;
        }

        medAbstractDilateImageProcessPluginManager& pluginManager(void)
        {
            return _private::manager;
        }

        medAbstractDilateImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }

        void initialize(const QString& path, bool verbose)
        {
            if(verbose)
                pluginManager().setVerboseLoading(verbose);
            pluginManager().initialize(path);
        }
    }
    namespace openingImage
    {
        namespace _private
        {
            medAbstractOpeningImageProcessPluginManager manager;
            medAbstractOpeningImageProcessPluginFactory factory;
        }

        medAbstractOpeningImageProcessPluginManager& pluginManager(void)
        {
            return _private::manager;
        }

        medAbstractOpeningImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }

        void initialize(const QString& path, bool verbose)
        {
            if(verbose)
                pluginManager().setVerboseLoading(verbose);
            pluginManager().initialize(path);
        }
    }
    namespace closingImage
    {
        namespace _private
        {
            medAbstractClosingImageProcessPluginManager manager;
            medAbstractClosingImageProcessPluginFactory factory;
        }

        medAbstractClosingImageProcessPluginManager& pluginManager(void)
        {
            return _private::manager;
        }

        medAbstractClosingImageProcessPluginFactory& pluginFactory(void)
        {
            return _private::factory;
        }

        void initialize(const QString& path, bool verbose)
        {
            if(verbose)
                pluginManager().setVerboseLoading(verbose);
            pluginManager().initialize(path);
        }
    }
}


}// end of medProcessLayer

