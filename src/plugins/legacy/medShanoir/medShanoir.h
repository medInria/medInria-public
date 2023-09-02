/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2023. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once
#include <QStringList>
#include <medAbstractSource.h>
#include <medStringParameter.h>

class Authenticator;
class RequestManager;
class LocalInfo;
class SyncNetwork;
class AsyncNetwork;

/**
 * @class medShanoir
 * @author Léo Filoche, Florent Leray
 * @brief The medShanoir class is a medAbstractSource that allows to connect to a Shanoir NG server.
 * The class is based on the Shanoir API. See more at https://github.com/fli-iam/shanoir-ng
 * @details
 * This class is used as an interface between medInria and the plugin's internal logic. It does not contain any logic by itself.
*/
class medShanoir : public medAbstractSource
{
	Q_OBJECT

public:
	/**
	 * @brief medShanoir constructor
	 * initializes the private members of the class, and connects their signals and slots.
	*/
	medShanoir();

	/**
	 * @brief medShanoir destructor
	 * deletes properly the private members of the class.
	 * @details
	 * Not used for now, because the members are deleted by the parent class.
	*/
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
	 * @return the medAbstractParameter that are volatil (their value are not saved by the app).
	*/
	QList<medAbstractParameter *> getVolatilParameters()   override;

	/**
	 * @return the medAbstractParameter that can be used to filter the data.
	*/
	QList<medAbstractParameter *> getFilteringParameters() override;

	/* ***********************************************************************/
	/* *************** Get source properties *********************************/
	/* ***********************************************************************/
	
	/**
	 * @return whether the source is writable or not. (false)
	*/
	bool isWritable() override;

	/**
	 * @return whether the source is local or not. (false)
	*/
	bool isLocal()    override;

	/**
	 * @return whether the source is cached or not. (false)
	*/
	bool isCached()   override;

	/**
	 * @return whether the plugin is connected to the server or not.
	*/
	bool isOnline()   override;

	/**
	 * @return whether the plugin fetches data by minimal entries or mandatory attributes. (true)
	*/
	bool isFetchByMinimalEntriesOrMandatoryAttributes() override;

	/**
	 * @return the IO interface of the source. (IO_FILE)
	*/
	int getIOInterface() override;

	/**
	 * @return a map that links the type of the data to the format of the data that can be read by the source. (here, only "Image" -> ".nii.gz")
	*/
	QMap<QString, QStringList> getTypeAndFormat() override;

	/* ***********************************************************************/
	/* *************** Get source structure information **********************/
	/* ***********************************************************************/
	
	/**
	 * @return the name of the plugin instance. It is set up by setInstanceName(QString) 
	*/
	QString getInstanceName() override;

	/**
	 * @return the id of the plugin instance. It is set up by initialization(QString)
	*/
	QString getInstanceId() override;

	/**
	 * @return the number of tree-view levels of the source. (7)
	*/
	unsigned int getLevelCount() override;

	/**
	 * @return the default writable level of the source. (7)
	 * @details it is important to note that users with lower privileges will not be able to write on the server.
	*/
	unsigned int getLevelDesiredWritable() override;

	/**
	 * @return the names of the levels of the source. ({"study", "subject", "examination", "dataset-acquisition", "dataset", "processingDataset", "processedDataset"})
	*/
	QStringList getLevelNames() override;

	/**
	 * @param pi_uiLevel id of a level
	 * @return the name of the pi_uiLevel
	*/
	QString getLevelName(unsigned int pi_uiLevel) override;

	/**
	 * @param pi_uiLevel id of a level
	 * @return whether the level is writable or not. 
	 * @details it is important to note that users with lower privileges will not be able to write on the server.
	*/
	bool isLevelWritable(unsigned int pi_uiLevel) override;

	/**
	 * @param pi_uiLevel id of a level
	 * @return the mandatory attributes of the level.
	 * @details for now, the mandatory attributes are the same as the minimal entries :
	 * "name", "type" &"format"
	*/
	QStringList getMandatoryAttributesKeys(unsigned int pi_uiLevel) override;

	/* ***********************************************************************/
	/* *************** Get elements data *************************************/
	/* ***********************************************************************/

	/**
	 * @param pi_uiLevel id of a level
	 * @param parentKey the key of a tree-view element
	 * @return the corresponding minimal entries (children) that are contained in the element of the level.
	 * @details the system of keys is used to identify the elements in the tree-view. Each level is identified by its id (an integer), and the key is a concatenation of the ids of the parents, separated by a dot. For instance, we a use key of the form studyId.subjectId.examinationId in order to designate an examination.
	 */
	QList<levelMinimalEntries> getMinimalEntries(unsigned int pi_uiLevel, QString parentKey) override;

	/**
	 * same goal as getMinimalEntries(), except that levelMinimalEntries are in the form of a QMap, and that other information can be added (date, owner, etc)
	 * @param i_uiLevel a tree-view level id 
	 * @param parentKey the key of a tree-view element
	 * @return the corresponding mandatory attributes of the element that are contained inside the level.
	 * @details for now, the mandatory attributes are the same as the minimal entries.
	 */
	QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, QString parentKey) override;

	/**
	 * has no effect for now. (returns false)
	*/
	bool getAdditionalAttributes(unsigned int pi_uiLevel, QString key, datasetAttributes &po_attributes) override;

	/* ***********************************************************************/
	/* *************** Get data          *************************************/
	/* ***********************************************************************/
	
	/**
	 * @param pi_uiLevel tree-view level id 
	 * @param key the key of a tree-view element
	 * @return the data of the element that is contained inside the level. (it can be empty if the element is a folder)
	 * @details this function is only useful for the datasets levels ("real" datasets and processed datasets - id 4 & 6). In those cases, it returns the path to the Nifti file that is contained in the dataset.
	 * The Nifti file is downloaded as a zip from the server, decompressed and stored in a temporary folder, that is deleted in reasonnable time after the function is called, or when the application is closed.
	 */
	QVariant getDirectData(unsigned int pi_uiLevel, QString key) override;

	/**
	 * asynchronous version of getDirectData()
	 * @param pi_uiLevel tree-view level id .
	 * @param key the key of a tree-view element
	 * @return id of the request 
	 */
	int getAssyncData(unsigned int pi_uiLevel, QString key) override;

	/* ***********************************************************************/
	/* *************** Store data          ***********************************/
	/* ***********************************************************************/

	/**
	 * @param data a path to a Nifti file.
	 * @param pio_minimalEntries the minimal entries of the element  to store.
	 * @param pi_uiLevel tree-view level id .
	 * @param parentKey the key of a tree-view element
	 * @return whether the operation was successful or not.
	 * @details this function is only useful for the datasets levels ("real" datasets and processed datasets - id 4 & 6). In those cases, it stores the path to the Nifti file that is contained in the dataset.
	 * The Nifti file is uploaded as a zip to the server, compressed and stored in a temporary folder, that is deleted in reasonnable time after the function is called, or when the application is closed.
	 */
	bool addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;

	/**
	 * asynchronous version of addDirectData()
	 * @param data a path to a Nifti file.
	 * @param pio_minimalEntries the minimal entries of the element  to store.
	 * @param pi_uiLevel tree-view level id .
	 * @param parentKey the key of a tree-view element
	 * @return id of the request 
	 */
	int addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;

	/**
	 * has no effect for now. (returns false)
	*/
	bool createPath(QList<levelMinimalEntries> &pio_path, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel = 0, QString parentKey = "") override;

	/**
	 * @param pio_minimalEntries the minimal entries of the element  to store.
	 * @param pi_attributes the attributes of the element to store. -- they are not used for now.
	 * @param pi_uiLevel tree-view level id .
	 * @param parentKey the key of a tree-view element
	 * @return whether the operation was successful or not.
	 * @details this function is only useful for the creation of processing datasets (id 5).
	 */
	bool createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey) override;

	/**
	 * has no effect for now. (returns false)
	*/
	bool alterMetaData(datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString key) override;

	/**
	 * has no effect for now. (returns false)
	*/
	bool getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key) override;

	/**
	 * has no effect for now. (returns false)
	*/
	bool setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key) override;

	/**
	 * has no effect for now. (returns false)
	 * @details the plugin is not caching anything for now, therefore this function is not used.
	*/
	bool commitData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;

	/**
	 * has no effect for now. (returns false)
	 * @details the plugin is not caching anything for now, therefore this function is not used.
	*/
	int push(unsigned int pi_uiLevel, QString key) override;

	/* ***********************************************************************/
	/* *************** Handle async results   ********************************/
	/* ***********************************************************************/

	/**
	 * @param pi_iRequest id of a request
	 * @return the results of the request if it exists and is finished. An empty QVariant otherwise.
	 * @details gets the results of an asynchronous request if it exists and is finished.
	*/
	QVariant getAsyncResults(int pi_iRequest) override;

	/* ***********************************************************************/
	/* *************** Optional functions  ***********************************/
	/* ***********************************************************************/

	/**
	 * has no effect for now. (returns false)
	 */
	inline medAbstractWritingPolicy *getWritingPolicy() { return nullptr; }

public slots:

	/**
	 * @param pi_iRequest id of a request
	 * @details aborts an asynchronous request if it exists and is not finished.
	*/
	void abort(int pi_iRequest) override;

private:
//members
	LocalInfo      * m_info;       //for the plugin's general information & instance management
	Authenticator  * m_authent;   //for authentication to the shanoir server
	SyncNetwork    * m_syncNet;    //for synchronous requests --managing the treeview structure, getting & adding data synchronously
	AsyncNetwork   * m_asyncNet;   //for asynchronous requests  --getting & adding data aysnchronously
	RequestManager * m_requester; //for network requests --is used by Authentificator, SyncNetwork & AsyncNetwork
};

