#pragma once

#include <Fetch.h>

#include <QMap>
#include <QString>

class FetchPost
	:public Fetch
{
private:
	QString m_url;
	QMap<QString, QString> m_headers;
	QMap<QString, QString> m_data;

public:
	FetchPost(int id, QString url, const QMap<QString, QString>& headers = QMap<QString, QString>(),const QMap<QString, QString>& formData= QMap<QString, QString>());

	void run() override;
};