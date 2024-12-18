#include <QString>
#include <QObject>

#include <medAbstractParameter.h>
#include <medStringParameter.h>
#include <medAbstractSource.h>

class Authenticator;
class medShanoir;
class RequestManager;

using levelMinimalEntries = medAbstractSource::levelMinimalEntries;

/**
 * @class LocalInfo
 * @brief This class is used to store the information that do not require a request to the shanoir server.
*/
class LocalInfo : public QObject
{
	Q_OBJECT

public:

//////////////////////////
////  Constructor/Destructor
//////////////////////////

    LocalInfo(medShanoir * parent);
    
    ~LocalInfo();

//////////////////////////
////  MedShanoir methods
//////////////////////////  
    
    bool initialization(QString const &pi_instanceId);

    bool setInstanceName(QString const &pi_instanceName);

    QList<medAbstractParameter *> getAllParameters();

    QList<medAbstractParameter *> getCipherParameters();

    QList<medAbstractParameter *> getVolatilParameters();

	QList<medAbstractParameter *> getFilteringParameters();

    bool isWritable() ;

	bool isLocal() ;
	
    bool isCached() ;

    bool isFetchByMinimalEntriesOrMandatoryAttributes();

    int getIOInterface();

    QMap<QString, QStringList> getTypeAndFormat();

    QString getInstanceName();

    QString getInstanceId();

    unsigned int getLevelCount();

    unsigned int getLevelDesiredWritable();

    QStringList getLevelNames();

    QString getLevelName(unsigned int pi_uiLevel);

    bool isLevelWritable(unsigned int pi_uiLevel);

    QStringList getMandatoryAttributesKeys(unsigned int pi_uiLevel);

    bool commitData(QVariant data, medAbstractSource::levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey);

public:

////////////////////////////
//// internal logic methods
////////////////////////////

    /**
     * @return the username that is entered for the plugin instance.
    */
    QString getUsername();

    /**
     * @return the password that is entered for the plugin instance.
    */
    QString getPassword();
    
    /**
     * @return the hostname that is entered for the plugin instance.
    */
    QString getDomain();

    /**
     * @return the root URL of the shanoir server for requests creation.
    */
    QString getBaseURL();

    /**
     * @return the path to the storage folder of the plugin instance.
    */
	QString getStoragePath();


private:
    // constants
    const QStringList s_level_names = {"study", "subject", "examination", "dataset-acquisition", "dataset", "processingDataset", "processedDataset"};

    // dynamic variables 
    QString m_instance_id;    // id of the plugin instance
	QString m_instance_name;  // name of the plugin instance

    // plugin instance parameters
	medStringParameter m_username; // username of the plugin instance
	medStringParameter m_password; // password of the plugin instance
	medStringParameter m_hostname; // hostname of the plugin instance
};