#include <medApplicationContext.h>

#include <medVirtualRepresentation.h>
#include <medDataHub.h>
#include <medNotifSys.h>
#include <medSourceHandler.h>
#include <medPluginManager.h>
#include <medDataManager.h>
#include <medApplication.h>
#include <medNewLogger.h>

medApplicationContext* medApplicationContext::m_pInstance = nullptr;

struct medApplicationContextPrivate
{
    medVirtualRepresentation* pVirtualRepresentation;
    medDataHub*               pDataHub;
    medNotifSys*              pNotifSys;
    medSourceHandler*         pSourceHandler;
    medPluginManager*         pPluginManager;
    medDataManager*           pDataManager;
    medApplication*           pApp;
    medNewLogger*             pLogger;
};

medApplicationContext* medApplicationContext::instance()
{
    if (m_pInstance == nullptr)
    {
        m_pInstance = new medApplicationContext();
    }

    return m_pInstance;
}

medApplicationContext::~medApplicationContext()
{
    delete d;
    m_pInstance = nullptr;
}

void medApplicationContext::setParent(medApplication * parent)
{
    QObject::setParent(parent);
    d->pApp = parent;
}

// *************************************************************************
// ******* Bloc of Getters                                             *****
// *************************************************************************
medVirtualRepresentation * medApplicationContext::getVirtualRepresentation()
{
    return d->pVirtualRepresentation;
}

medDataHub * medApplicationContext::getDataHub()
{
    return d->pDataHub;
}

medNotifSys * medApplicationContext::getNotifSys()
{
    return d->pNotifSys;
}

medSourceHandler * medApplicationContext::getSourceHandler()
{
    return d->pSourceHandler;
}

medPluginManager * medApplicationContext::getPluginManager()
{
    return d->pPluginManager;
}

medDataManager * medApplicationContext::getDataManager()
{
    return d->pDataManager;
}

medApplication * medApplicationContext::getApp()
{
    return d->pApp;
}

medNewLogger * medApplicationContext::getLogger()
{
    return d->pLogger;
}


// *************************************************************************
// ******* Bloc of Setters                                             *****
// *************************************************************************
void medApplicationContext::setVirtualRepresentation(medVirtualRepresentation *virtualRepresentation)
{
    d->pVirtualRepresentation = virtualRepresentation;
}

void medApplicationContext::setDataHub(medDataHub *dataHub)
{
    d->pDataHub = dataHub;
}

void medApplicationContext::setNotifSys(medNotifSys *notifSys)
{
    d->pNotifSys = notifSys;
}

void medApplicationContext::setSourceHandler(medSourceHandler *sourceHandler)
{
    d->pSourceHandler = sourceHandler;
}

void medApplicationContext::setPluginManager(medPluginManager *pluginManager)
{
    d->pPluginManager = pluginManager;
}

void medApplicationContext::setDataManager(medDataManager *dataManager)
{
    d->pDataManager = dataManager;
}

void medApplicationContext::setLogger(medNewLogger *logger)
{
    d->pLogger = logger;
}

QString medApplicationContext::getApplicationSettingsPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/" + QCoreApplication::organizationName() + "/" + QCoreApplication::applicationName();
}

// *************************************************************************
// ******* Bloc of privet                                              *****
// *************************************************************************
medApplicationContext::medApplicationContext()
{
    d = new medApplicationContextPrivate;

    d->pVirtualRepresentation = nullptr;
    d->pDataHub               = nullptr;
    d->pNotifSys              = nullptr;
    d->pSourceHandler         = nullptr;
    d->pPluginManager         = nullptr;
    d->pDataManager           = nullptr;
    d->pApp                   = nullptr;
    d->pLogger                = nullptr;
}
