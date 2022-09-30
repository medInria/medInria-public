#include "medNotif.h"
#include "medNotifSys.h"

#include <medNewLogger.h>

#include <QJsonObject>
#include <QJsonDocument>

class medNotifPrivate
{
public:
    notifLevel criticalityLevel;
    QString title;
    QString msg;
    int achievement;
    int displayTime;

    QString extraTrigger1;
    QString extraTrigger2;
    QString extraTrigger3;
};

std::shared_ptr<medNotif> medNotif::createNotif(notifLevel pi_criticityLevel, QString pi_title, QString pi_message, int pi_time_ms, int pi_achivementPercentage)
{
    std::shared_ptr<medNotif> notifRes;    
    
    if (pi_criticityLevel >= notifLevel::info && pi_criticityLevel <= notifLevel::error && !pi_title.isEmpty() && pi_achivementPercentage >= -2 && pi_achivementPercentage <= 100)
    {
        notifRes = std::shared_ptr<medNotif>(new medNotif());
    
        notifRes->d->criticalityLevel = pi_criticityLevel;
        notifRes->d->title = pi_title;
        notifRes->d->msg = pi_message;
    
        notifRes->d->achievement = pi_achivementPercentage;
        notifRes->d->displayTime = pi_time_ms;
    
        if (!medNotifSys::registerNotif(notifRes))
        {
            notifRes.reset();
        }
    }
    
    if (notifRes == nullptr)
    {
        mDebug << "medNotif can't create notification with : " << "critical level: " << (int) pi_criticityLevel << ", title: " << pi_title << ", message: " << pi_message << ", display time: " << pi_time_ms << ", percentage: " << pi_achivementPercentage;
    }

    return notifRes;
}

medNotif::medNotif()
{
    d = new medNotifPrivate();
}

medNotif::~medNotif()
{
    delete d;
}


bool medNotif::update(notifLevel pi_criticityLevel, int pi_achivementPercentage)
{
    bool bRes = pi_criticityLevel >= notifLevel::info && pi_criticityLevel <= notifLevel::error && pi_achivementPercentage >= -2 && pi_achivementPercentage <= 101;
    
    if (bRes)
    {
        d->criticalityLevel = pi_criticityLevel;
        d->achievement = pi_achivementPercentage;
        medNotifSys::updateNotif(this);
        emit notification();
    }
    else
    {
        mDebug << "medNotif (" << toString() << ") can't update with :" << "critical level: " << static_cast<int>(pi_criticityLevel) << ", percentage: " << pi_achivementPercentage;
    }

    return bRes;
}

notifLevel medNotif::getCriticalityLevel()
{
    return d->criticalityLevel;
}

QString medNotif::getTitle()
{
    return d->title;
}

QString medNotif::getMessage()
{
    return d->msg;
}

int medNotif::getAchievement()
{
    return d->achievement;
}

int medNotif::getDisplayTime()
{
    return d->displayTime;
}

void medNotif::setExtraTrigger1Label(QString const & label)
{
    d->extraTrigger1 = label;
    emit notification();
}

void medNotif::setExtraTrigger2Label(QString const & label)
{
    d->extraTrigger2 = label;
    emit notification();
}

void medNotif::setExtraTrigger3Label(QString const & label)
{
    d->extraTrigger3 = label;
    emit notification();
}

QString medNotif::getExtraTrigger1Label()
{
    return d->extraTrigger1;
}

QString medNotif::getExtraTrigger2Label()
{
    return d->extraTrigger2;
}

QString medNotif::getExtraTrigger3Label()
{
    return d->extraTrigger3;
}

QString medNotif::toString()
{
    QJsonObject notif;

    notif.insert("id", (QTextStream()<< static_cast<void*>(this)).readAll());
    notif.insert("criticalityLevel", static_cast<int>(d->criticalityLevel));
    notif.insert("title", d->title);
    notif.insert("msg", d->msg);
    notif.insert("achievement", d->achievement);
    notif.insert("time", d->displayTime);
    
    return QJsonDocument(notif).toJson();
}
