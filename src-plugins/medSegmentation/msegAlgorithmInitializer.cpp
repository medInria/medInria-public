/*
 * msegAlgorithmInitializer.cpp
 *
 *  Created on: 31 mai 2011 13:38:31
 *      Author: jstark
 */

#include "msegAlgorithmInitializer.h"

#include "msegAlgorithmConnectedThreshold.h"
#include "msegAlgorithmConnectedThresholdToolbox.h"
#include "medProcessPaintSegm.h"
#include "msegAlgorithmPaintToolbox.h"
#include <medToolBoxFactory.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

namespace mseg {
AlgorithmInitializer::AlgorithmInitializer(Controller * controller) :
    m_controller( controller )
{
}

AlgorithmInitializer::~AlgorithmInitializer()
{
}

bool AlgorithmInitializer::initialize()
{
    bool ret = initializeProcesses();
    ret &= initializeWidgets();
    return ret;
}

bool AlgorithmInitializer::initializeProcesses()
{
    dtkAbstractProcessFactory *factory = dtkAbstractProcessFactory::instance();

    bool ret (true);
    if (! factory->registerProcessType( AlgorithmConnectedThreshold::s_identifier(),
        AlgorithmConnectedThreshold::create, AlgorithmGeneric::ms_interfaceName ) ) {
        ret = false;
    }
    if (! factory->registerProcessType( medProcessPaintSegm::s_identifier(),
            medProcessPaintSegm::create, AlgorithmGeneric::ms_interfaceName ) ) {
        ret = false;
    }
    return ret;
}

bool AlgorithmInitializer::initializeWidgets()
{
    bool ret (true);
    medToolBoxFactory * factory = medToolBoxFactory::instance();
    //Commented out until bugs are fixed: #338 #339
//    ret &= factory->registerCustomToolBox(
//        AlgorithmConnectedThresholdToolbox::s_identifier(),
//        AlgorithmConnectedThresholdToolbox::s_name(),
//        AlgorithmConnectedThresholdToolbox::s_description(),
//        "segmentation",
//        AlgorithmConnectedThresholdToolbox::createInstance);

    ret &= factory->registerToolBox<AlgorithmPaintToolbox>(
        AlgorithmPaintToolbox::s_identifier(),
        AlgorithmPaintToolbox::s_name(),
        AlgorithmPaintToolbox::s_description(),
        QStringList()<<"segmentation");

    return ret;
}

} // namespace mseg
