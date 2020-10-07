#include <medITKImageDTKImageConverter.h>
#include <medITKImageDTKImageConverterPlugin.h>

#include <dtkCore>
#include <medCore.h>

void medITKImageDTKImageConverterPlugin::initialize(void)
{
    medCore::dataConverter::pluginFactory().record("medITKImageDTKImageConverterPlugin",medITKImageDTKImageConverterCreator);
}

void medITKImageDTKImageConverterPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medITKImageDTKImageConverter)

