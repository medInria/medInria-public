#include "FetchGet.h"

#include <QJsonObject>

#include <JsonReaderWriter.h>
#include <Network.h>


FetchGet::FetchGet(int id, QString url, const QMap<QString, QString>& headers)
	: Fetch(id), m_url(url), m_headers(headers)
{

}


void FetchGet::run()
{
    qDebug() << "Running GET fetch "<<m_id;
	Network net;
	emit fetchEnded(m_id, net.replyContentObject(net.httpGetFetch(m_url, m_headers)));
}