/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medTestExport.h>

#include <dtkCoreSupport/dtkTest.h>
#include <dtkCoreSupport/dtkSmartPointer.h>
#include <medAbstractData.h>

namespace medTest
{
    /**
     * Get the string contained in the environment
     * variable var. Returns false if the variable is empty or
     * not defined.
     */
    MEDTEST_EXPORT bool envString (const QString &var, QString &value);

    /**
     * Initialize the medPluginManager with paths defined
     * in MEDINRIA_PLUGINS_DIR and MEDINRIA_PLUGINS_DIR_LEGACY. Test will fail is environment
     * variable is not defined or empty.
     */
    MEDTEST_EXPORT void initializePlugins();

    /**
     * Clean-up medPluginManager.
     */
    MEDTEST_EXPORT void uninitializePlugins();

    /**
     * Initialize Qt resource system with resource file defined
     * in environment variable var.
     */
    MEDTEST_EXPORT void initializeResources(const QString &var);

    /**
     * Returns a medAbstractData instance by reading file.
     * Make sure that a db controller has been registered prior to
     * using this function.
     */
    MEDTEST_EXPORT dtkSmartPointer<medAbstractData> readData (const QString &file);

} // end of namespace


