#include <QString>
#include <QObject>
#include <QJsonObject>
#include <QTimer>

#include <QEventLoop>
#include <QUuid>
#include <QVariant>
#include <QPair>
#include <QNetworkRequest>


#include <RequestManager.h>

class LocalInfo;
class medShanoir;


class Authenticator : public QObject
{
    Q_OBJECT

public:
    Authenticator(medShanoir * parent, LocalInfo * info, RequestManager * requester);
    ~Authenticator();

	bool connect(bool pi_bEnable);

    void initAuthentication(const QString domain, const QString username, const QString password);
    void disauthenticate();

    bool isAuthenticated();

    QString getCurrentAccessToken();
    QString getCurrentDomain();

    /**
	 * @param tokenDurationRefreshment  a static function that calculates the frequency of the token refreshments (in milliseconds).
	 * @return the effective refreshing time.
	 */
	int autoRefreshAccessToken(int(*tokenDurationRefreshment)(int)=twoThirds);

signals:
	void authentPost(QUuid, QNetworkRequest, QByteArray);

public slots:
    void authentPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int httpCode);

private slots:
	void tokenUpdate(); 

private:
    bool accessTokenExpired();
	QUuid waitPostResult(QNetworkRequest &req, QByteArray &postData);

    static int twoThirds(int token_duration); 

private:
	LocalInfo * m_info;

    QMap<QUuid, QPair<QEventLoop*, RequestResponse>> m_requestMap;

    QJsonObject m_current_token;
    QTimer m_timer;
    QString m_domain; // domain the authenticater is currently connected on. Empty String if it is offline.    
};
