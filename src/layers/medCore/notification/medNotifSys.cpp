#include <medNotifSys.h>
#include <medNotif.h>

#include <atomic>

#include <QJsonObject>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QApplication>


class medNotifSysPrivate
{
public:
    static medNotifSys *instance;
    QList<medUsrNotif> notifList;
    QSystemTrayIcon *sysTray;
    bool sysTrayOnNonFocus;
    bool focus;
};

medNotifSys* medNotifSysPrivate::instance = new medNotifSys();



medNotifSys* medNotifSys::instance()
{
    return medNotifSysPrivate::instance;
}

medNotifSys::medNotifSys(QObject * parent) : d(new medNotifSysPrivate())
{
    d->sysTray = nullptr;
    d->sysTrayOnNonFocus = true;
    d->focus = false;

    qRegisterMetaType<medUsrNotif>("medUsrNotif");

    //connect(this, &medNotifSys::notification, this, &medNotifSys::debugWriteNotifications);
}

medNotifSys::~medNotifSys()
{
}



bool medNotifSys::registerNotif(medUsrNotif & notif)
{
    return medNotifSysPrivate::instance->add(notif);
}

bool medNotifSys::unregisterNotif(medUsrNotif const & notif)
{
    return medNotifSysPrivate::instance->remove2(notif);
}

void medNotifSys::updateNotif()
{
    medNotif* notif = dynamic_cast<medNotif*>(sender());
    if (notif)
    {
        for (auto & usrNotif : d->notifList)
        {
            if (usrNotif.get() == notif)
            {
                emit  update(&usrNotif);
            }
        }
    }
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

bool medNotifSys::add(medUsrNotif & notif)
{
    bool bRes = false;

    if (!d->notifList.contains(notif))
    {
         d->notifList.push_back(notif);
        auto * pNotif = notif.get();
        //auto conn1 = connect(pNotif, &medNotif::updated, [=]() {updateNotif(this->getUsrNotif(pNotif)); });
        auto conn1 = connect(pNotif, &medNotif::updated, this, &medNotifSys::updateNotif);
        //auto conn2 = connect(pNotif, &medNotif::destroyed, [=]() {remove(this->getUsrNotif(pNotif)); });
        auto conn2 = connect(pNotif, &medNotif::destroyed, this, &medNotifSys::remove);

        emit notification(&d->notifList.back());

        osNotif(notif);
        if (notif->getDisplayTime() > 0)
        {
            QTimer::singleShot(notif->getDisplayTime(),  [&,this]() {unregisterNotif(notif); });
        }

        bRes = true;
    }

    return bRes;
}

bool medNotifSys::remove()
{
    bool bRes = false;

    medNotif* notif = dynamic_cast<medNotif*>(sender());
    if (notif)
    {
        bool bFound = false;
        bool bContinue;
        auto it = d->notifList.begin();
        auto itEnd = d->notifList.end();

        bContinue = it != itEnd;
        while (bContinue)
        {
            if (it->get() == notif)
            {
                bFound = true;
                bContinue = false;
            }
        }

        if (bFound)
        {
            bRes = d->notifList.removeOne(*it);
            if (bRes)
            {
                emit removed(*it);
            }
        }
    }

    return bRes;
}

bool medNotifSys::remove2(medUsrNotif const & notif)
{
    bool bRes = false;


    bool bFound = false;
    bool bContinue;
    auto it = d->notifList.begin();
    auto itEnd = d->notifList.end();

    bContinue = it != itEnd;
    while (bContinue)
    {
        if (it->get() == notif.get())
        {
            bFound = true;
            bContinue = false;
        }
    }

    if (bFound)
    {
        bRes = d->notifList.removeOne(*it);
        if (bRes)
        {
            emit removed(*it);
        }
    }


    return bRes;
}

void medNotifSys::windowOnTop(bool pi_bOntop)
{
    d->focus = pi_bOntop;
}

void medNotifSys::osNotif(medUsrNotif & notif)
{
    if (d->sysTray)
    {
        if (!d->sysTrayOnNonFocus || !d->focus)
        {
            d->sysTray->showMessage(notif->getTitle(), notif->getMessage());
        }
    }
}

medUsrNotif medNotifSys::getUsrNotif(medNotif * notif)
{
    for (auto usrNotif : d->notifList)
    {
        if (usrNotif.get() == notif)
        {
            return usrNotif;
        }
    }
    return medUsrNotif();
}