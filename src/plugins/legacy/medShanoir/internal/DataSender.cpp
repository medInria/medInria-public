#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <JsonReaderWriter.h>
#include <QJsonDocument>
#include <QDateTime>

#include "DataSender.h"

DataSender::DataSender(int id, Authenticater & auth, QObject *parent) : QObject(parent), m_id(id), m_auth(auth)
{
}

int DataSender::getId()
{
    return m_id;
}