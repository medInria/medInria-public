#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QObject>
#include <QDateTime>
#include <QTime>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "Authenticater.h"
#include <JsonReaderWriter.h>
#include <SettingsManager.h>

Authenticater::Authenticater(Network & network,const QString username, const QString password)
	:Authenticater(network)
{
	initAuthentication(username, password);
}

Authenticater::Authenticater(Network & network)
	:m_net(network)
{
	m_config = JsonReaderWriter::fileToJsonDocument("Config/auth_config.json").object();
	m_current_token = SettingsManager::retrieveJson(ACCESS_TOKEN_KEY);
}

void Authenticater::initAuthentication(const QString username, const QString password)
{
	/** CREATING THE POST REQUEST **/
	QString url = m_config.value("url").toString();
	QMap<QString, QString> headers = JsonReaderWriter::jsonObjectToQmap(m_config.value("headers").toObject());
	QMap<QString, QString> data = JsonReaderWriter::jsonObjectToQmap(m_config.value("data-login").toObject());

	data["username"] = username;
	data["password"] = password;

	/** SENDING THE POST REQUEST **/
	QNetworkReply *reply = m_net.httpPostFetch(url, headers, data);
	QJsonObject response = m_net.replyContentObject(reply);
	if (!JsonReaderWriter::verifyKeys(response, ACCESS_TOKEN_KEYS_TO_VERIFY, ACCESS_TOKEN_KEY_TYPES_TO_VERIFY))
	{
		qDebug() << "AUTHENTICATION FAILED";
		qDebug()<<"REQUEST RESPONSE STATUS : "<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		return;
	}
	/** TOKEN RECOVERY **/
	m_current_token = response;
	/** TOKEN REFRESH AUTOMATION */
	autoRefreshAccessToken();
}


bool Authenticater::isAuthenticated()
{
	return !m_current_token.isEmpty() && !accessTokenExpired();
}

bool Authenticater::accessTokenExpired()
{
	QDateTime exp_dt = QDateTime::fromString(m_current_token.value(ACCESS_TOKEN_EXPIRATION_KEY).toString(), Qt::ISODate);
	QDateTime now = QDateTime::currentDateTime();
	return now>=exp_dt;
}


QString Authenticater::getCurrentAccessToken()
{
	return m_current_token.value("access_token").toString();
}

int Authenticater::autoRefreshAccessToken(int(*tokenDurationRefreshment)(int))
{
	int token_duration = m_current_token.value("expires_in").toInt(); // in seconds
	connect(&m_timer, &QTimer::timeout, this, &Authenticater::tokenUpdate);
	// the timer will be called every time the token reaches the expiration time
	int refresh_time = tokenDurationRefreshment(token_duration*1000); // in ms
	qDebug().noquote() << "THE ACCESS TOKEN WILL BE UPDATED EVERY" << QTime(0, 0).addMSecs(refresh_time).toString("m'mins' s'secs'");
	m_timer.start(refresh_time);
	tokenUpdate(); 
	return refresh_time;
}
void Authenticater::disauthenticate()
{
	SettingsManager::reset(ACCESS_TOKEN_KEY);
	m_current_token = QJsonObject();
	qDebug()<<"\nTHE SESSION HAS ENDED\n";
}

int Authenticater::twoThirds(int token_duration)
{
	return (2 * ((int)token_duration / 3));
}

void Authenticater::tokenUpdate()
{
	QString url = m_config.value("url").toString();
	QMap<QString, QString> headers = JsonReaderWriter::jsonObjectToQmap(m_config.value("headers").toObject());
	QMap<QString, QString> data = JsonReaderWriter::jsonObjectToQmap(m_config.value("data-refresh").toObject());
	data["refresh_token"] = m_current_token.value("refresh_token").toString();

	QDateTime fetching_datetime = QDateTime::currentDateTime();
	QJsonObject response = m_net.replyContentObject(m_net.httpPostFetch(url, headers, data));
	if (JsonReaderWriter::verifyKeys(response, ACCESS_TOKEN_KEYS_TO_VERIFY))
	{
		m_current_token = response;
		// TOKEN STORAGE 
		int token_duration = m_current_token.value("expires_in").toInt();
		QDateTime token_expiration_datetime = fetching_datetime.addSecs(token_duration);
		m_current_token[ACCESS_TOKEN_EXPIRATION_KEY] = token_expiration_datetime.toString(Qt::ISODate);
		SettingsManager::reset(ACCESS_TOKEN_KEY);
		SettingsManager::storeJson(ACCESS_TOKEN_KEY, m_current_token);

		qDebug().noquote() << QDateTime::currentDateTime().toString() << "--THE ACCESS TOKEN HAS BEEN UPDATED. IT IS NOW AVAILABLE UNTIL " << token_expiration_datetime.toString();
	}
	else
	{
		qDebug() << "THE ACCESS TOKEN UPDATE HAS FAILED";
	}
}