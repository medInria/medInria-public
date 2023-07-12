#include "FetchPost.h"

#include <QDebug>

#include <Network.h>
#include <JsonReaderWriter.h>

FetchPost::FetchPost(int id, QString url, const QMap<QString, QString>& headers, const QMap<QString, QString>& formData)
	: Fetch(id), m_url(url), m_headers(headers), m_data(formData)
{
}

void FetchPost::run()
{
	qDebug() << "Running POST fetch " << m_id;
	Network net;
	emit fetchEnded(m_id,net.replyContentObject(net.httpPostFetch(m_url, m_headers, m_data)));
}
