#include <QString>
#include <QObject>

#include <medAbstractParameter.h>
#include <medStringParameter.h>
#include <medAbstractSource.h>

class Authenticator;
class medShanoir;
class RequestManager;

using levelMinimalEntries = medAbstractSource::levelMinimalEntries;

class LocalInfo : public QObject
{
	Q_OBJECT

public:
    LocalInfo(medShanoir * parent);
    ~LocalInfo();

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
    QString getUsername();
    QString getPassword();
    QString getDomain();
    QString getBaseURL();

	QString getStoragePath();


private:
    // constants
    const QStringList s_level_names = {"study", "subject", "examination", "dataset-acquisition", "dataset", "processingDataset", "processedDataset"};

    // dynamic variables 
    QString m_instance_id;
	QString m_instance_name;
	medStringParameter m_username;
	medStringParameter m_password;
	medStringParameter m_hostname;
};