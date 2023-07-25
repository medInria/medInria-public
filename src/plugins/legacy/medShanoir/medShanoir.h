#pragma once
#include <QStringList>
#include <medAbstractSource.h>
#include <medStringParameter.h>

#include <Authenticater.h>
#include <Network.h>
#include <RequestManager.h>

#include <Levels.h>

class ShanoirPlugin : public medAbstractSource
{
	Q_OBJECT

public: 
// needs to be public for the connection of signals (data reception)
	RequestManager m_rm;

private:
	// Local variables
	Network m_net;
	Authenticater m_auth;
	

	const QStringList s_level_names = {"root", "study", "subject", "examination", "dataset-acquisition", "dataset"};

	// Variables accessible via getter declared in medAbstractSource
	QString m_instance_id;
	QString m_instance_name;
	medStringParameter m_username;
	medStringParameter m_password;
	medStringParameter m_hostname;

	/**
	 * @return the minimalEntries of the available studies from the logged account. 
	*/
	QList<medAbstractSource::levelMinimalEntries> getRootMinimalEntries();
	/**
	 * @param key is a string of an integer (the study id), representing the study key
	 * @return the minimalEntries of the subjects contained in the study.
	 */
	QList<medAbstractSource::levelMinimalEntries> getStudyMinimalEntries(QString key);
	/**
	 * @param keyParent a string of the format studyId.subjectId representing the subject key
	 * @return the minimalEntries of the examinations contained in the subject.
	 */
	QList<medAbstractSource::levelMinimalEntries> getSubjectMinimalEntries(QString keyParent);

	/**
	 * @param keyParent a string of the format studyId.subjectId.examinationId representing the examination key
	 * @return the minimalEntries of the dataset-acquisitions contained in the examination.
	 */
	QList<medAbstractSource::levelMinimalEntries> getExaminationMinimalEntries(QString keyParent);
	/**
	 * @param keyParent a string of the format studyId.subjectId.examinationId.datasetAcqId representing the dataset-acquisition key
	 * @return the minimalEntries of the datasets contained in the examination.
	 */
	QList<medAbstractSource::levelMinimalEntries> getDatasetAcquisitionMinimalEntries(QString keyParent);

	/**
	 * converts a list of minimal entries to a list of QMap of the form {key,name,description,type}
	 */
	static QList<QMap<QString, QString>> entriesToMandatoryAttributes(const QList<medAbstractSource::levelMinimalEntries> &entries);

public:
	ShanoirPlugin();
	virtual ~ShanoirPlugin();

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
	QList<medAbstractParameter *> getVolatilParameters() override;
	QList<medAbstractParameter *> getFilteringParameters() override;

	/* ***********************************************************************/
	/* *************** Get source properties *********************************/
	/* ***********************************************************************/
	bool isWritable() override;
	bool isLocal() override;
	bool isCached() override;
	bool isOnline() override;
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

	void dataReception(int idRequest);

};
