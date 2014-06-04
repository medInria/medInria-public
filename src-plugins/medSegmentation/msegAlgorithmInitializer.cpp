/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <msegAlgorithmInitializer.h>

#include <msegAlgorithmPaintToolbox.h>
#include <medToolBoxFactory.h>

#include <dtkCore/dtkAbstractProcessFactory.h>

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
    return true;
}

bool AlgorithmInitializer::initializeWidgets()
{
    bool ret (true);
    medToolBoxFactory * factory = medToolBoxFactory::instance();

    ret &= factory->registerToolBox<AlgorithmPaintToolbox>();

    return ret;
}
