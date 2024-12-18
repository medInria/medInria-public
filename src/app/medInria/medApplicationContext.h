#pragma once

#include <QObject>



#include <medVirtualRepresentation.h>
#include <medDataHub.h>
#include <medNotifSys.h>
#include <medSourceHandler.h>
#include <medPluginManager.h>
#include <medDataManager.h>
#include <medApplication.h>
#include <medNewLogger.h>

//class medVirtualRepresentation;
//class medDataHub;
//class medNotifSys;
//class medSourceHandler;
//class medPluginManager;
//class medDataManager;
//class medApplication;
//class medNewLogger;

class medApplicationContextPrivate;

class medApplicationContext : public QObject
{
    Q_OBJECT
public:

    static medApplicationContext* instance();
    ~medApplicationContext();

    void setParent(medApplication* parent);

    medVirtualRepresentation* getVirtualRepresentation();
    medDataHub*               getDataHub();
    medNotifSys*              getNotifSys();
    medSourceHandler*         getSourceHandler();
    medPluginManager*         getPluginManager();
    medDataManager*           getDataManager();
    medApplication*           getApp();
    medNewLogger*             getLogger();


    void setVirtualRepresentation(medVirtualRepresentation*);
    void setDataHub(medDataHub*);
    void setNotifSys(medNotifSys*);
    void setSourceHandler(medSourceHandler*);
    void setPluginManager(medPluginManager*);
    void setDataManager(medDataManager*);
    void setLogger(medNewLogger*);

    static QString getApplicationSettingsPath();

private:
    medApplicationContext();
    medApplicationContextPrivate *d;
    static medApplicationContext *m_pInstance;
};
