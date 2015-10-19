#include "medManualSegmentation.h"
#include "medManualSegmentationPlugin.h"

#include <dtkCore>
#include "medCore.h"

// ///////////////////////////////////////////////////////////////////
// medManualSegmentationPlugin
// ///////////////////////////////////////////////////////////////////

void medManualSegmentationPlugin::initialize(void)
{
    medCore::segmentation::pluginFactory().record("medManualSegmentation", medManualSegmentationCreator);
}

void medManualSegmentationPlugin::uninitialize(void)
{

}

// ///////////////////////////////////////////////////////////////////
// Plugin meta data
// ///////////////////////////////////////////////////////////////////

DTK_DEFINE_PLUGIN(medManualSegmentation)

