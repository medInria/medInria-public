#include <medNotifSys.h>
#include <medNotif.h>

#include <atomic>

#include <QJsonObject>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QApplication>

//#include <medNewLogger.h>

#include <QFile> //TODO remove after debug phase

class medNotifSysPrivate
{
public:
    static medNotifSys *instance;
    QList<medUsrNotif> notifList;
    QSystemTrayIcon *sysTray;
    bool sysTrayOnNonFocus;
    bool focus;

    QFile debugFile;
};

medNotifSys* medNotifSysPrivate::instance = new medNotifSys();



medNotifSys* medNotifSys::instance()
{
    return medNotifSysPrivate::instance;
}

medNotifSys::medNotifSys(QObject * parent) : d(new medNotifSysPrivate())
{
    d->sysTray = nullptr;    
    d->debugFile.setFileName("debugNotification.json");
    d->debugFile.open(QIODevice::WriteOnly);
    d->sysTrayOnNonFocus = true;
    d->focus = false;
    connect(this, &medNotifSys::notification, this, &medNotifSys::debugWriteNotifications);
}

medNotifSys::~medNotifSys()
{
    d->debugFile.close();
}



bool medNotifSys::registerNotif(medUsrNotif notif)
{
    bool bRes = false;

    if (!medNotifSysPrivate::instance->d->notifList.contains(notif))
    {
        medNotifSysPrivate::instance->d->notifList.push_back(notif);
        emit medNotifSysPrivate::instance->notification(notif.get());
        medNotifSysPrivate::instance->osNotif(notif.get());
        if (notif->getDisplayTime() > 0)
        {
            auto *pRawNotif = notif.get();
            QTimer::singleShot(notif->getDisplayTime(), medNotifSysPrivate::instance, [=]() {medNotifSysPrivate::instance->unregisterNotif(pRawNotif); });
        }

        bRes = true;
    }    

    return bRes;
}

bool medNotifSys::unregisterNotif(medNotif *notif)
{
    bool bRes = false;

    medUsrNotif usrNotif;
    auto &list = medNotifSysPrivate::instance->d->notifList;

    for (auto &val : list)
    {
        if (val.get() == notif)
        {
            usrNotif = val;
            bRes = true;
        }
    }

    if (usrNotif != nullptr)
    {
        int nb1 = usrNotif.use_count();
        list.removeOne(usrNotif);
        emit medNotifSysPrivate::instance->removed(notif);
        int nb2 = usrNotif.use_count();
    }

    return bRes;
}

void medNotifSys::updateNotif(medNotif *notif)
{
    emit  medNotifSysPrivate::instance->update(notif);
}

void medNotifSys::setOperatingSystemNotification(bool pi_bOn)
{
    if (pi_bOn)
    {
        if (d->sysTray == nullptr)
        {
            d->sysTray = new QSystemTrayIcon(QIcon(":/medInria.ico"), this);
            d->sysTray->setVisible(true);
        }
    }
    else
    {
        delete d->sysTray;
        d->sysTray = nullptr;
    }
}

void medNotifSys::setOSNotifOnlyNonFocus(bool pi_bOn)
{
    d->sysTrayOnNonFocus = pi_bOn;
}

void medNotifSys::remove(medNotif * pi_pNotif)
{
    //TODO remove after debug phase
    d->debugFile.write("\r\nRemove notif:\r\n");
    d->debugFile.write(pi_pNotif->toString().toUtf8());
}

void medNotifSys::windowOnTop(bool pi_bOntop)
{
    d->focus = pi_bOntop;
}

void medNotifSys::osNotif(medNotif * notif)
{
    if (d->sysTray)
    {
        if (!d->sysTrayOnNonFocus || !d->focus)
        {
            medNotifSysPrivate::instance->d->sysTray->showMessage(notif->getTitle(), notif->getMessage());
        }
    }
}

void medNotifSys::debugWriteNotifications(medNotif *notif)
{
    d->debugFile.write(notif->toString().toUtf8()); //TODO remove after debug phase
}
