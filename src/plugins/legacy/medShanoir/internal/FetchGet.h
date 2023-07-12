#pragma once

#include <QMap>
#include <QString>

#include <Fetch.h>

class FetchGet : public Fetch
{
private:
	QString m_url;
	QMap<QString, QString> m_headers;

public:
	FetchGet(int id, QString url, const QMap<QString, QString>& headers = QMap<QString, QString>());
	void run() override;
};