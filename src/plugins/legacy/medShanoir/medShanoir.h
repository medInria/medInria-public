#pragma once
#include <QStringList>
#include <medAbstractSource.h>
#include <medStringParameter.h>

class Authenticator;
class RequestManager;
class LocalInfo;
class SyncNetwork;
class AsyncNetwork;

class medShanoir : public medAbstractSource
{
	Q_OBJECT

public:
	medShanoir();
	virtual ~medShanoir();

	/* ***********************************************************************/
	/* *************** Init/set/ctrl source properties ***********************/
	/* ***********************************************************************/
	/**
	 * sets up the instance id if pi_instanceId is valid
	 * @param pi_instanceId an instance id.
	 * @return the success of the update operation.
	*/
	bool initialization(QString const &pi_instanceId) override;

	/**
	 * sets up the instance name if pi_instanceName is valid
	 * @param pi_instanceName an instance name
	 * @return the success of the update operation.
	*/
	bool setInstanceName(QString const &pi_instanceName) override;

	/**
	 * @param pi_Enable indicates whether the method connects or disconnects
	 * @return the success of the operation
	 */
	bool connect(bool pi_bEnable = true) override;

	/**
	 * @return all the medAbstractParameter that are used by the source.
	*/
	QList<medAbstractParameter *> getAllParameters() override;

	/**
	 * @return the medAbstractParameter that need to be ciphered by the application.
	*/
	QList<medAbstractParameter *> getCipherParameters() override;

	/**
	 * @return the medAbstractParameter that can be forgotten on close of the app.
	*/
	QList<medAbstractParameter *> getVolatilParameters()   override;
	QList<medAbstractParameter *> getFilteringParameters() override;

	/* ***********************************************************************/
	/* *************** Get source properties *********************************/
	/* ***********************************************************************/
	bool isWritable() override;
	bool isLocal()    override;
	bool isCached()   override;
	bool isOnline()   override;
	bool isFetchByMinimalEntriesOrMandatoryAttributes() override;

	int getIOInterface() override;

	QMap<QString, QStringList> getTypeAndFormat() override;

	/* ***********************************************************************/
	/* *************** Get source structure information **********************/
	/* ***********************************************************************/
	QString getInstanceName() override;
	QString getInstanceId() override;

	unsigned int getLevelCount() override;
	unsigned int getLevelDesiredWritable() override;
	QStringList getLevelNames() override;
	QString getLevelName(unsigned int pi_uiLevel) override;

	bool isLevelWritable(unsigned int pi_uiLevel) override;

	QStringList getMandatoryAttributesKeys(unsigned int pi_uiLevel) override;

	/* ***********************************************************************/
	/* *************** Get elements data *************************************/
	/* ***********************************************************************/

	/**
	 * @param pi_uiLevel integer indicating the level of entry you want to access to (Study, Examination, Dataset..). Min value : 0. Max value : 4
	 * @param parentKey a key, based on the ids of the parents. For instance, we use key of the form studyId.subjectId.examinationId in order to retrieve an examination content.
	 * @return the corresponding minimal entries that are contained in the element of the level.
	 */
	QList<levelMinimalEntries> getMinimalEntries(unsigned int pi_uiLevel, QString parentKey) override;

	/**
	 * same goal as getMinimalEntries().
	 * Except that levelMinimalEntries are in the form of a QMap. And that other information can be added (date, owner, etc)
	 * @param i_uiLevel integer indicating the level of entry you want to access to (Study, Examination, Dataset..). Min value : 0. Max value : 4
	 * @param parentKey a key, based on the ids of the parents. For instance, we a use key of the form studyId.subjectId.examinationId in order to retrieve an examination content.
	 * @return the corresponding mandatory attributes of the element that are contained inside  of the level.
	 */
	QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, QString parentKey) override;

	/**
	 *
	 * has no effect for now. (returns false)
	*/
	bool getAdditionalAttributes(unsigned int pi_uiLevel, QString key, datasetAttributes &po_attributes) override;

	/* ***********************************************************************/
	/* *************** Get data          *************************************/
	/* ***********************************************************************/

	/**
	 * For now, loads from Shanoir a zip file containing a .nii or .dcm and save it on the file system.
	 * The QVariant that is returned is the path of the zip file.
	*/
	QVariant getDirectData(unsigned int pi_uiLevel, QString key) override;
	int getAssyncData(unsigned int pi_uiLevel, QString key) override;

	/* ***********************************************************************/
	/* *************** Store data          ***********************************/
	/* ***********************************************************************/

	/**
	 *
	 * @param data  a  QString of a path to a .nii file
	 * @param pio_minimalEntries
	 * @param pi_uiLevel
	 * @param parentKey
	 * @return the success of the operation
	 */
	bool addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;
	int addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;

	bool createPath(QList<levelMinimalEntries> &pio_path, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel = 0, QString parentKey = "") override;
	bool createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey) override;

	bool alterMetaData(datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString key) override;

	bool getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key) override;
	bool setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key) override;

	bool commitData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;
	int push(unsigned int pi_uiLevel, QString key) override;

	/* ***********************************************************************/
	/* *************** Handle async results   ********************************/
	/* ***********************************************************************/
	QVariant getAsyncResults(int pi_iRequest) override;

	/* ***********************************************************************/
	/* *************** Optional functions  ***********************************/
	/* ***********************************************************************/
	inline medAbstractWritingPolicy *getWritingPolicy() { return nullptr; }

public slots:
	void abort(int pi_iRequest) override;

private:
	LocalInfo      * m_info;
	SyncNetwork    * m_syncNet;
	AsyncNetwork   * m_asyncNet;
	RequestManager * m_requester;
	Authenticator  * m_authent;
};

