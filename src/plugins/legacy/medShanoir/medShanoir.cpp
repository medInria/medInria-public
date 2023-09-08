#include "medShanoir.h"

#include <QObject>

#include <LocalInfo.h>
#include <RequestManager.h>
#include <Authenticator.h>
#include <SyncNetwork.h>
#include <AsyncNetwork.h>


medShanoir::medShanoir()
{
	// initializing the objects
	m_info      = new LocalInfo(this);
	m_requester = new RequestManager(this);
	m_authent   = new Authenticator (this, m_info, m_requester);
	m_syncNet   = new SyncNetwork   (this, m_info, m_authent, m_requester);
	m_asyncNet  = new AsyncNetwork  (this, m_info, m_authent, m_syncNet, m_requester);

	// connecting the network objects to the request manager

	QObject::connect(m_requester, &RequestManager::responseHttpPost, m_authent, &Authenticator::authentPostSlot, Qt::QueuedConnection);
	
	QObject::connect(m_requester, &RequestManager::responseHttpGet,   m_syncNet, &SyncNetwork::syncRequestSlot,   Qt::QueuedConnection);
	QObject::connect(m_requester, &RequestManager::responseHttpPost,  m_syncNet, &SyncNetwork::syncRequestSlot,  Qt::QueuedConnection);
	QObject::connect(m_requester, &RequestManager::responseHttpPut,   m_syncNet, &SyncNetwork::syncRequestSlot,   Qt::QueuedConnection);
	
	QObject::connect(m_requester, &RequestManager::responseHttpGet,  m_asyncNet, &AsyncNetwork::asyncGetSlot,  Qt::QueuedConnection);
	QObject::connect(m_requester, &RequestManager::responseHttpPost, m_asyncNet, &AsyncNetwork::asyncPostSlot, Qt::QueuedConnection);
	QObject::connect(m_requester, &RequestManager::responseHttpPut,  m_asyncNet, &AsyncNetwork::asyncPutSlot,  Qt::QueuedConnection);
}

medShanoir::~medShanoir()
{
}

/* ***********************************************************************/
/* *************** Init/set/ctrl source properties ***********************/
/* ***********************************************************************/
bool medShanoir::initialization(QString const  &pi_instanceId)
{
	return m_info->initialization(pi_instanceId);
}

bool medShanoir::setInstanceName(QString const &pi_instanceName)
{
	return m_info->setInstanceName(pi_instanceName);
}

bool medShanoir::connect(bool pi_bEnable)
{
	bool res=  m_authent->connect(pi_bEnable);
	emit connectionStatus(isOnline());
	return res;
}

QList<medAbstractParameter*> medShanoir::getAllParameters()
{
	return m_info->getAllParameters();
}

QList<medAbstractParameter*> medShanoir::getCipherParameters() 
{
	return m_info->getCipherParameters();
}

QList<medAbstractParameter*> medShanoir::getVolatilParameters() 
{
	return m_info->getVolatilParameters();
}

QList<medAbstractParameter*> medShanoir::getFilteringParameters()
{
	return m_info->getFilteringParameters();
}

/* ***********************************************************************/
/* *************** Get source properties *********************************/
/* ***********************************************************************/
bool medShanoir::isWritable()
{
	return m_info->isWritable();
}

bool medShanoir::isLocal()
{
	return m_info->isLocal();
}

bool medShanoir::isCached()
{
	return m_info->isCached();
}

bool medShanoir::isOnline()
{
	return m_authent->isAuthenticated();
}

bool medShanoir::isFetchByMinimalEntriesOrMandatoryAttributes()
{
	return m_info->isFetchByMinimalEntriesOrMandatoryAttributes();
}


int medShanoir::getIOInterface()
{
	return m_info->getIOInterface();
}


QMap<QString, QStringList> medShanoir::getTypeAndFormat()
{
	return m_info->getTypeAndFormat();
}


/* ***********************************************************************/
/* *************** Get source structure information **********************/
/* ***********************************************************************/
QString          medShanoir::getInstanceName()
{
	return m_info->getInstanceName();
}

QString          medShanoir::getInstanceId()
{
	return m_info->getInstanceId();
}

unsigned int     medShanoir::getLevelCount()
{
	return m_info->getLevelCount();
}

unsigned int    medShanoir::getLevelDesiredWritable()
{
	return m_info->getLevelDesiredWritable();
}

QStringList      medShanoir::getLevelNames()
{
	return m_info->getLevelNames();
}

QString          medShanoir::getLevelName(unsigned int pi_uiLevel)
{
	return m_info->getLevelName(pi_uiLevel);
}

bool             medShanoir::isLevelWritable(unsigned int pi_uiLevel) {
	return m_info->isLevelWritable(pi_uiLevel);
}

QStringList      medShanoir::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
{
	return m_info->getMandatoryAttributesKeys(pi_uiLevel);
}


/* ***********************************************************************/
/* *************** Get elements data *************************************/
/* ***********************************************************************/

QList<medAbstractSource::levelMinimalEntries>    medShanoir::getMinimalEntries(unsigned int pi_uiLevel, QString parentKey)
{
    return m_syncNet->getMinimalEntries(pi_uiLevel, parentKey);
}

QList<QMap<QString, QString>> medShanoir::getMandatoryAttributes(unsigned int pi_uiLevel, QString parentKey)
{
	return m_syncNet->getMandatoryAttributes(pi_uiLevel, parentKey);
}

bool  medShanoir::getAdditionalAttributes(unsigned int pi_uiLevel, QString key, datasetAttributes &po_attributes)
{
	return false;
}


/* ***********************************************************************/
/* *************** Get data          *************************************/
/* ***********************************************************************/
QVariant medShanoir::getDirectData(unsigned int pi_uiLevel, QString parentKey)
{
	return m_syncNet->getDirectData(pi_uiLevel, parentKey);
}

int      medShanoir::getAssyncData(unsigned int pi_uiLevel, QString parentKey)
{
	//WARNING: we cannot download the freshly uploaded data, the returned parentKey worth 'true' and not the parentKey as expected. (reload medInria and it is fixed)
	return m_asyncNet->getAssyncData(pi_uiLevel, parentKey);
}

/* ***********************************************************************/
/* *************** Store data          ***********************************/
/* ***********************************************************************/
bool medShanoir::addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{	
	// mock for create folder (dev only)
	if (pi_uiLevel == 5)
	{
		datasetAttributes const &pi_attributes = {};
		return m_syncNet->createFolder(pio_minimalEntries, pi_attributes, pi_uiLevel, parentKey);
	}
	else
	{
		return m_syncNet ->addDirectData(data, pio_minimalEntries, pi_uiLevel, parentKey); 
	}
}

int  medShanoir::addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
	// mock for create folder (dev only)
	//WARNING: to see and manipulate properly the created folder, you must reload medInria, otherwise the parentKey will be the instance name of the plugin
	// by example you cannot add a processed dataset on a freshly created processing dataset. 
	// if (pi_uiLevel == 5)
	// {
	// 	datasetAttributes const &pi_attributes = {};
	// 	m_syncNet->createFolder(pio_minimalEntries, pi_attributes , pi_uiLevel, parentKey);
	// }
	return m_asyncNet -> addAssyncData(data, pio_minimalEntries, pi_uiLevel, parentKey); 
}

bool medShanoir::createPath(QList<levelMinimalEntries> &pio_path, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey)
{
	return false;
}

bool medShanoir::createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey)
{
	return m_syncNet->createFolder(pio_minimalEntries, pi_attributes, pi_uiLevel, parentKey);
}


bool medShanoir::alterMetaData(datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString key)
{
	return false;
}

bool medShanoir::getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key)
{
	return false;
}

bool medShanoir::setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key)
{
	return false;
}

bool medShanoir::commitData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey)
{
	return m_info->commitData(data, pio_minimalEntries, pi_uiLevel, parentKey);
}

int  medShanoir::push(unsigned int pi_uiLevel, QString key)
{
	return 0;
}

/* ***********************************************************************/
/* *************** Handle async results   ********************************/
/* ***********************************************************************/
QVariant medShanoir::getAsyncResults(int pi_iRequest)
{
	return m_asyncNet->getAsyncResults(pi_iRequest);
}

/* ***********************************************************************/
/* *************** Optional functions  ***********************************/
/* ***********************************************************************/


void medShanoir::abort(int pi_iRequest)
{
	m_asyncNet->abort(pi_iRequest);
}
