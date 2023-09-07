#include <QStandardPaths>
#include <QCoreApplication>

#include "LocalInfo.h"

#include <medShanoir.h>
#include <medStringParameter.h>

LocalInfo::LocalInfo(medShanoir * parent) :QObject(parent), m_username("username"), m_password("password"), m_hostname("hostname")
{
    m_password.setDefaultRepresentation(4);
    m_hostname.setValue("shanoir.irisa.fr");
}

LocalInfo::~LocalInfo()
{

}


bool LocalInfo::initialization(QString const  &pi_instanceId)
{
    bool instance_id_exists = !pi_instanceId.isEmpty();
	if(instance_id_exists)
	{
		m_instance_id = pi_instanceId;
	}
	return instance_id_exists;
}

bool LocalInfo::setInstanceName(QString const &pi_instanceName)
{
	bool instance_name_exists = !pi_instanceName.isEmpty(); 
	if(instance_name_exists)
	{
		m_instance_name = pi_instanceName;
	}
	return instance_name_exists;
}


QList<medAbstractParameter*> LocalInfo::getAllParameters()
{
	return  { &m_hostname, &m_username, &m_password};
}

QList<medAbstractParameter*> LocalInfo::getCipherParameters() 
{
	return {&m_password};
}

QList<medAbstractParameter*> LocalInfo::getVolatilParameters() 
{
	return {};
}

QList<medAbstractParameter*> LocalInfo::getFilteringParameters()
{
	return {};
}

bool LocalInfo::isWritable()
{
	return false;
}

bool LocalInfo::isLocal()
{
	return false;
}

bool LocalInfo::isCached()
{
	return false;
}

bool LocalInfo::isFetchByMinimalEntriesOrMandatoryAttributes()
{
	return true;
}

int LocalInfo::getIOInterface()
{
	return IO_FILE;
}

QMap<QString, QStringList> LocalInfo::getTypeAndFormat()
{
	QMap<QString, QStringList> map;
	map["Image"] = QStringList({ ".nii.gz" }); 
	return map;
}

QString          LocalInfo::getInstanceName()
{
	return m_instance_name;
}

QString          LocalInfo::getInstanceId()
{
	return m_instance_id;
}

unsigned int     LocalInfo::getLevelCount()
{
	return s_level_names.size();
}

unsigned int    LocalInfo::getLevelDesiredWritable()
{
	// 7 is the index level of the processed datasets.
	// it is this type of dataset that shanoir allows to create
	// (if we have the necessary rights)
	return 7; 
}

QStringList      LocalInfo::getLevelNames()
{
	return s_level_names;
}



QString          LocalInfo::getLevelName(unsigned int pi_uiLevel)
{
	// checking that the pi_uiLevel is valid 
	if(pi_uiLevel < getLevelCount())
	{
		return s_level_names[pi_uiLevel];
	}
	return "";
}

bool             LocalInfo::isLevelWritable(unsigned int pi_uiLevel) {
	// we can write processing datasets and processed datasets
	return pi_uiLevel == 5 || pi_uiLevel == 6;
}

QStringList      LocalInfo::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
	// checking that the pi_uiLevel is valid
	if(pi_uiLevel < getLevelCount())
	{
		// the keys are the same as the minimal entries. For now, nothing is added
		return { "key", "name", "description" };
	}
	else
	{
		return {};
	}
}

bool LocalInfo::commitData(QVariant data, medAbstractSource::levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
	return false;
}

QString LocalInfo::getUsername()
{
    return m_username.value();
}

QString LocalInfo::getPassword()
{
    return m_password.value();
}

QString LocalInfo::getDomain()
{
    return m_hostname.value();
}

QString LocalInfo::getBaseURL()
{
	return  "https://" + getDomain() + "/shanoir-ng/";
}

QString LocalInfo::getStoragePath()
{
	// the files are stored on a temporary folder, given that they aim to be loaded quickly after creation. 
	return QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + QCoreApplication::applicationName() + "/shanoirfiles/";
}
