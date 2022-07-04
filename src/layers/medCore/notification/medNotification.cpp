#include <medNotification.h>

#include <atomic>

#include <QJsonObject>
#include <QTimer>

class medNotificationPrivate
{
public:
    static medNotification *instance;
    QMap<int, QJsonObject> notifMap;
    std::atomic<unsigned short> atomic_count{ 0 };
};

medNotification* medNotificationPrivate::instance = nullptr;

medNotification::medNotification(QObject * parent) : d(new medNotificationPrivate())
{
    medNotificationPrivate::instance = this;
}

medNotification::~medNotification()
{
}

int medNotification::notify(notifLevel pi_level, QString pi_title, QString pi_message, int pi_time_ms, int pi_achievementPercentage, int pi_notifId)
{
    return medNotificationPrivate::instance->notify_internal(pi_level, pi_title, pi_message, pi_time_ms, pi_achievementPercentage, pi_notifId);
}

int medNotification::infoWithProgress(QString pi_title, QString pi_message, int pi_time_ms, int pi_achivementPercentage, int pi_notifId)
{
    return notify(notifLevel::info, pi_title, pi_message, pi_time_ms, pi_achivementPercentage, pi_notifId);
}

void medNotification::info(QString pi_title, QString pi_message, int pi_time_ms)
{
    notify(notifLevel::info, pi_title, pi_message, pi_time_ms);
}

void medNotification::success(QString pi_title, QString pi_message, int pi_time_ms)
{
    notify(notifLevel::success, pi_title, pi_message, pi_time_ms);
}

void medNotification::warning(QString pi_title, QString pi_message, int pi_time_ms)
{
    notify(notifLevel::warnning, pi_title, pi_message, pi_time_ms);
}

void medNotification::error(QString pi_title, QString pi_message)
{
    notify(notifLevel::error, pi_title, pi_message);
}

void medNotification::remove(int pi_id)
{
    d->notifMap.remove(pi_id);
}

int medNotification::notify_internal(notifLevel pi_level, QString pi_title, QString pi_message, int pi_time_ms, int pi_achievementPercentage, int pi_notifId)
{
    int idRes = 0;

    if (pi_notifId == 0)
    {
        idRes = ++d->atomic_count;
    }
    else if (d->notifMap.contains(pi_notifId))
    {
        idRes = pi_notifId;
    }

    if (idRes > 0)
    {
        QJsonObject &notif = d->notifMap[idRes];

        notif.insert("id", idRes);
        notif.insert("level", static_cast<int>(pi_level));
        notif.insert("title", pi_title);
        notif.insert("msg", pi_message);
        notif.insert("time", pi_time_ms);
        notif.insert("achievement", pi_achievementPercentage);

        emit notification(notif);
        if (pi_time_ms > 0)
        {
            QTimer::singleShot(pi_time_ms, this, [=]() {this->remove(idRes); });
        }
    }

    return idRes;
}
