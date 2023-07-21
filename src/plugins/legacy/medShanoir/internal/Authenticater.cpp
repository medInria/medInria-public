#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QObject>
#include <QDateTime>
#include <QTime>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <medNotif.h>

#include "Authenticater.h"
#include <JsonReaderWriter.h>
#include <SettingsManager.h>


Authenticater::Authenticater(Network & network)
	:m_net(network), m_domain("")
{
	m_current_token = SettingsManager::retrieveJson(ACCESS_TOKEN_KEY);
}

void Authenticater::initAuthentication(const QString domain, const QString username, const QString password)
{
	QString url = "https://"+domain+"/auth/realms/shanoir-ng/protocol/openid-connect/token";
	/** CREATING THE POST REQUEST **/
	QMap<QString, QString> headers = JsonReaderWriter::jsonObjectToQmap(AUTH_HEADERS);
	QMap<QString, QString> data = JsonReaderWriter::jsonObjectToQmap(DATA_LOGIN);

	data["username"] = username;
	data["password"] = password;

	/** SENDING THE POST REQUEST **/
	QNetworkReply *reply = m_net.httpPostFetch(url, headers, data);
	QJsonObject response = m_net.replyContentObject(reply);
	int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (!JsonReaderWriter::verifyKeys(response, ACCESS_TOKEN_KEYS_TO_VERIFY, ACCESS_TOKEN_KEY_TYPES_TO_VERIFY))
	{
		qDebug() << "AUTHENTICATION HAS FAILED";
		qDebug()<<"REQUEST RESPONSE STATUS : "<<status;
		QString notif_message = "The authentication to Shanoir failed.\n Request status : "+QString::number(status);
		if(status==401)
		{
			notif_message="\nPlease check your username and password.";
			qDebug() << response;
		}
		medNotif::createNotif(notifLevel::error, "Authentication to Shanoir", notif_message);
		return;
	}
	/** TOKEN RECOVERY **/
	m_current_token = response;
	m_domain = domain;
	medNotif::createNotif(notifLevel::success, "Authentication to Shanoir", "Successfully connected to "+getCurrentDomain());
	/** TOKEN REFRESH AUTOMATION */
	autoRefreshAccessToken();
}


bool Authenticater::isAuthenticated()
{
	return !m_current_token.isEmpty() && m_domain.size()>0 && !accessTokenExpired();
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
	m_timer.stop();
	medNotif::createNotif(notifLevel::info, "Authentication to Shanoir", "Successfully disconnected from "+getCurrentDomain());
	m_domain = "";
}

QString Authenticater::getCurrentDomain()
{
	return m_domain;
}

int Authenticater::twoThirds(int token_duration)
{
	return (2 * ((int)token_duration / 3));
}

void Authenticater::tokenUpdate()
{
	QString url = "https://" + m_domain + "/auth/realms/shanoir-ng/protocol/openid-connect/token";
	QMap<QString, QString> headers = JsonReaderWriter::jsonObjectToQmap(AUTH_HEADERS);
	QMap<QString, QString> data = JsonReaderWriter::jsonObjectToQmap(DATA_REFRESH);
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
		medNotif::createNotif(notifLevel::error, "Authentication to Shanoir", "Something went wrong while updating the access token. You will be disconnected.");
	}
}