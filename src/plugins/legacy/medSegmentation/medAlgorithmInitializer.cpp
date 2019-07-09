/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAlgorithmInitializer.h>

#include <medAlgorithmPaintToolbox.h>
#include <medToolBoxFactory.h>


medAlgorithmInitializer::medAlgorithmInitializer()
{
}

medAlgorithmInitializer::~medAlgorithmInitializer()
{
}

bool medAlgorithmInitializer::initialize()
{
    bool ret = initializeProcesses();
    ret &= initializeWidgets();
    return ret;
}

bool medAlgorithmInitializer::initializeProcesses()
{
    return true;
}

bool medAlgorithmInitializer::initializeWidgets()
{
    bool ret (true);
    medToolBoxFactory * factory = medToolBoxFactory::instance();

    ret &= factory->registerToolBox<medAlgorithmPaintToolbox>();

    return ret;
}
