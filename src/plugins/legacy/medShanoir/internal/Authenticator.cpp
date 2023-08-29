#include <QNetworkRequest>
#include <QTimer>

#include <medNotif.h>
#include <medShanoir.h>

#include <LocalInfo.h>
#include <JsonHelper.h>

#include "Authenticator.h"


Authenticator::Authenticator(medShanoir * parent, LocalInfo * info, RequestManager * requester) :  QObject(parent), m_info(info), m_domain("")
{
	QObject::connect(this, &Authenticator::authentPost, requester, &RequestManager::httpPost, Qt::ConnectionType::QueuedConnection);
}

Authenticator::~Authenticator()
{

}


bool Authenticator::connect(bool pi_bEnable)
{
	if(pi_bEnable)
 	{
 		if(isAuthenticated())
 		{ // THE TOKEN IS STILL EFFECTIVE : A REFRESH IS SUFFICIENT
 			autoRefreshAccessToken();
 		}
 		else
 		{ // THE TOKEN EXPIRED OR IT IS THE FIRST TIME WE CONNECT
 			initAuthentication(m_info->getDomain(), m_info->getUsername(), m_info->getPassword());
 		}
 	}
 	else 
 	{
 		disauthenticate();
 	}
 	return  pi_bEnable== isAuthenticated();
}

void Authenticator::initAuthentication(const QString domain, const QString username, const QString password)
{
    QString url = "https://"+domain+"/auth/realms/shanoir-ng/protocol/openid-connect/token";
    // construction of the qnetwork request
    QNetworkRequest req(url);
	req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
	QByteArray postData;
	postData.append("client_id=" + QUrl::toPercentEncoding("shanoir-uploader"));
	postData.append("&grant_type=" + QUrl::toPercentEncoding("password"));
	postData.append("&scope=" + QUrl::toPercentEncoding("offline_access"));
	postData.append("&username=" + QUrl::toPercentEncoding(username));
	postData.append("&password=" + QUrl::toPercentEncoding(password));


    // sending the request
	QUuid netReqId = waitPostResult(req, postData);

	// receiving the response
	RequestResponse res = m_requestMap[netReqId].second;
	m_requestMap.remove(netReqId);

	// handling the results
	if (!verifyJsonKeys(qbytearrayToQJson(res.payload), { "access_token", "expires_in","refresh_token"}, {"String", "Number", "String"}))
	{
		qDebug() << "AUTHENTICATION HAS FAILED";
		qDebug() << "REQUEST RESPONSE STATUS : " << res.code;
		QString notif_message = "The authentication to Shanoir failed.\n Request status : " + QString::number(res.code);
		if (res.code == 401)
		{
			notif_message = "\nPlease check your username and password.";
			qDebug() << res.payload;
		}
		medNotif::createNotif(notifLevel::error, "Authentication to Shanoir", notif_message);
		return;
	}
	/** TOKEN RECOVERY **/
	m_current_token = qbytearrayToQJson(res.payload);
	m_domain = domain;
	medNotif::createNotif(notifLevel::success, "Authentication to Shanoir", "Successfully connected to " + getCurrentDomain());
	/** TOKEN REFRESH AUTOMATION */
	autoRefreshAccessToken();
}

QUuid Authenticator::waitPostResult(QNetworkRequest &req, QByteArray &postData)
{
	QUuid netReqId = QUuid::createUuid();

	QEventLoop *waiter = new QEventLoop(this);
	m_requestMap[netReqId].first = waiter;
	emit authentPost(netReqId, req, postData);

	waiter->exec();

	return netReqId;
}

void Authenticator::disauthenticate()
{
    m_current_token = QJsonObject();
	m_timer.stop();
	medNotif::createNotif(notifLevel::info, "Authentication to Shanoir", "Successfully disconnected from "+getCurrentDomain());
	m_domain = "";
}

bool Authenticator::isAuthenticated()
{
    return !m_current_token.isEmpty() && m_domain.size()>0 && !accessTokenExpired();
}

QString Authenticator::getCurrentAccessToken()
{
    return  m_current_token.value("access_token").toString();
}

QString Authenticator::getCurrentDomain()
{
    return m_domain;
}

int Authenticator::autoRefreshAccessToken(int(*tokenDurationRefreshment)(int))
{
	int token_duration = m_current_token.value("expires_in").toInt(); // in seconds
	QObject::connect(&m_timer, &QTimer::timeout, this, &Authenticator::tokenUpdate);
	// the timer will be called every time the token reaches the expiration time
	int refresh_time = tokenDurationRefreshment(token_duration*1000); // in ms
	qDebug().noquote() << "THE ACCESS TOKEN OF "<<getCurrentDomain().toUpper()<<" WILL BE UPDATED EVERY" << QTime(0, 0).addMSecs(refresh_time).toString("m'mins' s'secs'");
	m_timer.start(refresh_time);
	tokenUpdate(); 
	return refresh_time;
}


bool Authenticator::accessTokenExpired()
{
    QDateTime exp_dt = QDateTime::fromString(m_current_token.value("expiration_datetime").toString(), Qt::ISODate);
	QDateTime now = QDateTime::currentDateTime();
	return now>=exp_dt;
}

int Authenticator::twoThirds(int token_duration)
{
	return (2 * ((int)token_duration / 3));
}

void Authenticator::authentPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int httpOrStatusCode)
{
	if (m_requestMap.contains(netReqId))
	{
		if (httpOrStatusCode < 100)
		{ // Request in progress
			int bytesSent = headers["bytesSent"].toInt();
			int bytesTotal = headers["bytesTotal"].toInt();
		}
		else
		{ // Request ended
			RequestResponse res = { httpOrStatusCode, headers, payload };
			m_requestMap[netReqId].second = res;
			m_requestMap[netReqId].first->exit();
		}
	}
}


void Authenticator::tokenUpdate()
{
	//// construction of the qnetworkrequest
    QString url = "https://" + m_domain + "/auth/realms/shanoir-ng/protocol/openid-connect/token";
	QNetworkRequest req(url);
	req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

	QByteArray postData;
	postData.append("client_id="+QUrl::toPercentEncoding("shanoir-uploader"));
	postData.append("&grant_type="+ QUrl::toPercentEncoding("refresh_token"));
	postData.append("&refresh_token="+  QUrl::toPercentEncoding(m_current_token.value("refresh_token").toString()));

 	// sending the request
	QUuid netReqId = waitPostResult(req, postData);

	// handling the response from the request
	QDateTime fetching_datetime = QDateTime::currentDateTime();
   
	QJsonObject newToken = qbytearrayToQJson(m_requestMap[netReqId].second.payload);

	if (verifyJsonKeys(newToken, { "access_token", "expires_in","refresh_token" }))
	{
		m_current_token = newToken;
		// adding the datetime of expiration inside the object
		int token_duration = m_current_token.value("expires_in").toInt();
		QDateTime token_expiration_datetime = fetching_datetime.addSecs(token_duration);
		 m_current_token["expiration_datetime"] = token_expiration_datetime.toString(Qt::ISODate);

		qDebug().noquote() << QDateTime::currentDateTime().toString() << "--THE ACCESS TOKEN OF" << getCurrentDomain().toUpper() << "HAS BEEN UPDATED. IT IS NOW AVAILABLE UNTIL " << token_expiration_datetime.toString();
	}
	else
	{
		qDebug() << "THE ACCESS TOKEN UPDATE HAS FAILED";
		m_current_token = QJsonObject();
		medNotif::createNotif(notifLevel::error, "Authentication to Shanoir", "Something went wrong while updating the access token of " + getCurrentDomain() + ". You will be disconnected.");
	}
}