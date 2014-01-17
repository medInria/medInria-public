/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "msegAlgorithmInitializer.h"

#include <msegAlgorithmConnectedThreshold.h>
#include <msegAlgorithmConnectedThresholdToolbox.h>
#include <medProcessPaintSegm.h>
#include <msegAlgorithmPaintToolbox.h>
#include <medToolBoxFactory.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

namespace mseg {
AlgorithmInitializer::AlgorithmInitializer()
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

    ret &= factory->registerToolBox<AlgorithmPaintToolbox>(
        AlgorithmPaintToolbox::s_identifier(),
        AlgorithmPaintToolbox::s_name(),
        AlgorithmPaintToolbox::s_description(),
        QStringList()<<"segmentation");

    return ret;
}

} // namespace mseg
