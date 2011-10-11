/*
 * msegAlgorithmInitializer.cpp
 *
 *  Created on: 31 mai 2011 13:38:31
 *      Author: jstark
 */

#include "msegAlgorithmInitializer.h"

#include "msegAlgorithmConnectedThreshold.h"
#include "msegAlgorithmConnectedThresholdParametersWidget.h"

#include "msegParametersWidgetFactory.h"
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
    if (! factory->registerProcessType( AlgorithmConnectedThreshold::s_typeName(),
        AlgorithmConnectedThreshold::create, AlgorithmGeneric::ms_interfaceName ) ) {
        ret = false;
    }
    return ret;
}

bool AlgorithmInitializer::initializeWidgets()
{
    bool ret (true);
    ParametersWidgetFactory * factory = ParametersWidgetFactory::instance();

    ret &= factory->registerWidget(AlgorithmConnectedThresholdParametersWidget::s_description(),
        AlgorithmConnectedThresholdParametersWidget::s_localizedName(),
        AlgorithmConnectedThresholdParametersWidget::createAlgorithmParametersWidget);

    return ret;
}

} // namespace mseg
