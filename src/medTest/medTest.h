#pragma once

#include "medTestExport.h"

#include <dtkCore/dtkTest.h>
#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkAbstractData.h>

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
     * in MEDINRIA_PLUGINS_PATH. Test will fail is environment
     * variable is not defined or empty.
     */
    MEDTEST_EXPORT void initializePlugins(void);

    /**
     * Clean-up medPluginManager.
     */
    MEDTEST_EXPORT void uninitializePlugins(void);

    /**
     * Initialize Qt resource system with resource file defined
     * in environment variable var.
     */
    MEDTEST_EXPORT void initializeResources(const QString &var);

    /**
     * Returns a dtkAbstractData instance by reading file.
     * Make sure that a db controller has been registered prior to
     * using this function.
     */
    MEDTEST_EXPORT dtkSmartPointer<dtkAbstractData> readData (const QString &file);

} // end of namespace


