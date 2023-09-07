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

/**
 * @class Authenticator
 * @brief The Authenticator class is used for the authentication task of the plugin instance.
 * @details 
 * This class aims to provide a simple way to authenticate the plugin instance to the Shanoir server, and to keep the authentication token up to date. 
 * The requests are synchronous. 
*/
class Authenticator : public QObject
{
    Q_OBJECT

public:

    /**
     * @param parent the plugin instance.
     * @param info the plugin instance info.
     * @param requester the request manager.
     * @brief Authenticator constructor.
     */
    Authenticator(medShanoir * parent, LocalInfo * info, RequestManager * requester);
    
    /**
     * @brief Authenticator destructor.
    */
    ~Authenticator();

//////////////////////////
////  MedShanoir methods
//////////////////////////

	bool connect(bool pi_bEnable);

    bool isAuthenticated(); // == isOnline()

//////////////////////////

    /**
     * @return the access token of the plugin instance.
    */
    QString getCurrentAccessToken();

signals:

    /**
     * @brief signal that is sent for the request manager to send the post request.
     */
	void authentPost(QUuid, QNetworkRequest, QByteArray);

public slots:

    /**
     * @brief slot received when a post request is running to indicate about its status.
    */
    void authentPostSlot(QUuid netReqId, QByteArray payload, QJsonObject headers, int httpCode);

private:
    /**
     * @param domain the domain of the server to connect to.
     * @param username the username to use for the authentication.
     * @param password the password to use for the authentication.
     * @brief initializes the authentication process and keeps the authentication token up to date.
     */
    void initAuthentication(const QString domain, const QString username, const QString password);

    /**
     * @brief disconnects the plugin instance from the server.
     */
    void disauthenticate();

    
    /**
     * @param token_duration the full duration of the token before it expires (in milliseconds).
     * @return two thirds of the token duration.
    */
    static int twoThirds(int token_duration); 

    /**
     * @param tokenDurationRefreshment  a static function that calculates the frequency of the token refreshments (in milliseconds).
     * @return the effective refreshment frequency the token will be refreshed at.
     */
    int autoRefreshAccessToken(int (*tokenDurationRefreshment)(int) = twoThirds);

    /**
     * @return true if the access token is expired, false otherwise.
     */
    bool accessTokenExpired();

    /**
     * @param req the request to send.
     * @param postData the data to send.
     * @return the request id.
     * @brief waitPostResult is a waiter function that sends a post request and waits for the response.
     */
	QUuid waitPostResult(QNetworkRequest &req, QByteArray &postData);

private slots:

    /**
     * @brief updates the current authentication token.
     */
	void tokenUpdate(); 

private:
	LocalInfo * m_info;  // pointer to the plugin instance info. Used for knowing the username, password and domain the users entered

    QMap<QUuid, QPair<QEventLoop*, RequestResponse>> m_requestMap; // map of the active requests sent for the authentication process

    QJsonObject m_current_token; // the current authentication token (empty if the plugin instance is offline)
    QTimer m_timer;   // timer used to refresh the token
    QString m_domain; // domain the authenticater is currently connected on. Empty String if it is offline.    
};
