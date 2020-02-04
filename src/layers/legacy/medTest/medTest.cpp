/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTest.h>

#include <medPluginManager.h>
#include <medDataManager.h>
#include <medDataIndex.h>

namespace medTest
{
    bool envString (const QString &var, QString &value)
    {
        value = qgetenv (var.toLatin1().constData()).constData();
        if (value.isEmpty()) {
            return false;
            dtkDebug() << var + " is not defined";
        }
        return true;
    }

    void initializePlugins ()
    {
        medPluginManager::instance()->setVerboseLoading (true);
        QString paths;
        QVERIFY(medTest::envString ("MEDINRIA_PLUGINS_DIR_LEGACY", paths)); // returns if env. variable is not defined
        medPluginManager::instance()->initialize();
        medPluginManager::instance()->printPlugins();
    }

    void uninitializePlugins ()
    {
        medPluginManager::instance()->uninitialize();
    }

    void initializeResources (const QString &var)
    {
        QString value;
        QVERIFY(medTest::envString (var, value));
        QResource::registerResource(value.toLatin1().constData());
    }

    dtkSmartPointer<medAbstractData> readData (const QString &file)
    {
        return nullptr;
    }

} // end of namespace
