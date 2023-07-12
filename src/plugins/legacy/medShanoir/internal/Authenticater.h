#pragma once
#include <QString>
#include <QJsonObject>
#include <QTimer>
#include <QObject>
#include <QStringList>

#include <Network.h>

class Authenticater	: public QObject
{
	Q_OBJECT

private:
	Network & m_net;
	QJsonObject m_config;
	QJsonObject m_current_token;
	QTimer m_timer;

	const QString ACCESS_TOKEN_KEY = "AuthToken";
	const QString ACCESS_TOKEN_EXPIRATION_KEY = "expiration_datetime";
	const QStringList ACCESS_TOKEN_KEYS_TO_VERIFY = { "access_token", "expires_in","refresh_token"};
	const QStringList ACCESS_TOKEN_KEY_TYPES_TO_VERIFY = {"String", "Number", "String"};

	bool accessTokenExpired();

	static int twoThirds(int token_duration);

public:
	Authenticater(Network & net);

	Authenticater(Network & net,const QString username, const QString password);

	void initAuthentication(const QString username, const QString password);

	bool isAuthenticated();

	QString getCurrentAccessToken();

	/**
	 * .
	 * 
	 * @param tokenDurationRefreshment  a static function that calculates the frequency of the token refreshments (in milliseconds).
	 * @return the effective refreshing time.
	 */
	int autoRefreshAccessToken(int(*tokenDurationRefreshment)(int)=twoThirds);

	void disauthenticate();


private slots:
	void tokenUpdate();

};

