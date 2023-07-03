#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include <medAbstractParameter.h>

#include <medCoreExport.h>

class medAbstractWritingPolicy;

/** @enum entryType
 *  @brief This is an enum class to indicate if a node in model and treeview represents a dataset, a folder or both (a dataset with children)
 */
enum entryType { dataset = 0/**< this is a dataset */, folder = 1/**< this is a folder */, both = 2/**< this is a dataset with children*/};

/**
 * @class medAbstractSource
 * @brief medAbstractSource is pure virtual class which defines the general API for sources.
 * @details 
 * A source is a repository of datasets, for example a file system, a PACS, a database, etc.<br>
 * A source is represented by a tree with a definite or infinite number of levels (-1).<br>
 * A source can be local, with fast access and transfer times, or remote with non-guaranteed performance. There are two behaviors associated with this notion:
 *   - For a local source medInria will favor synchronous access and requests
 *   - For remote sources, asynchronous access will be preferred.
 * 
 * Three types of information can be exchanged between the source and medInria through this class:
 *   - The minimalEntries which are absolutely necessary metadata to represent the source in a treeview.
 *   - Optional metadata that provides additional information for a dataset or a node.
 *   - A dataset which can itself be transferred in the form of a local file path, an instance of medAbstractData or a memory stream.
 * 
 * A source has settings for connecting to and accessing it.
 */
class MEDCORE_EXPORT medAbstractSource : public QObject
{
    Q_OBJECT

public:
    enum entryType {dataset = 0, folder = 1, both = 2};
    static constexpr const char* entryTypeToString(entryType e) throw()
    {
        switch (e)
        {
            case entryType::dataset: return "dataset";
            case entryType::folder: return "folder";
            case entryType::both: return "both";
            default: throw std::invalid_argument("Unimplemented item");
        }
    }

    struct datasetAttributes
    {
        QMap<QString, QString> values; // <keyName, value>
        QMap<QString, QString> tags;   // <keyName, tag value>
    };
    using  listAttributes = QList<datasetAttributes>;

    #define IO_FILE 1
    #define IO_MEDABSTRACTDATA 2
    #define IO_STREAM 4

    /**
     * @brief This structure represents the minimal data useful to build the tree. Minimal entries are a sub-part of mandatories attributes.
     */
    struct levelMinimalEntries
    {
        QString key; // TODO : change
        QString name;
        QString description;
        entryType type;
    };

    struct levelMinimalKeys
    {
        QString key;
        QString name;
        QString description;
        QString type;
    };

	
    enum eRequestStatus : int
    {
        aborted = -3,
        cnxLost = -2,
        faild = -1,
        finish = 0,
        pending = 1
    };

    /* ***********************************************************************/
    /* *************** Init/set/ctrl source properties ***********************/
    /* ***********************************************************************/
    virtual bool initialization(QString const  &pi_instanceId) = 0;
    virtual bool setInstanceName(QString const &pi_instanceName) = 0;
    virtual bool connect(bool pi_bEnable = true) = 0;


    virtual QList<medAbstractParameter*> getAllParameters() = 0;
    virtual QList<medAbstractParameter*> getCipherParameters() = 0;
    virtual QList<medAbstractParameter*> getVolatileParameters() = 0;
    virtual QList<medAbstractParameter*> getFilteringParameters() = 0;

    /* ***********************************************************************/
    /* *************** Get source properties *********************************/
    /* ***********************************************************************/
    
    virtual bool isWritable()  = 0;
    virtual bool isLocal()     = 0;
    virtual bool isCached()    = 0;
    virtual bool isOnline()    = 0;
    virtual bool isFetchByMinimalEntriesOrMandatoryAttributes() = 0; //true: minimalEntries, false: MandatoryAttributes
    virtual int getIOInterface() = 0; //return TOTO_FILE;// | TOTO_ABSDATA | TOTO_OTHER;
    virtual QMap<QString, QStringList> getTypeAndFormat() = 0; //<TypeOfData, ListOfSupportedFormat>
    
    /* ***********************************************************************/
    /* *************** Get source structure information **********************/
    /* ***********************************************************************/
    virtual QString          getInstanceName() = 0;
    virtual QString          getInstanceId()   = 0;
                             
    virtual unsigned int     getLevelCount() = 0;
    virtual unsigned int     getLevelDesiredWritable() = 0;
    virtual QStringList      getLevelNames() = 0;
    virtual QString          getLevelName(unsigned int pi_uiLevel) = 0;

    virtual bool             isLevelWritable(unsigned int pi_uiLevel) = 0;

    //virtual void             getMinimalKeys(unsigned int pi_uiLevel, QString &key, QString &name, QString &description) = 0;
    //virtual levelMinimalKeys getMinimalKeys(unsigned int pi_uiLevel) = 0;
    virtual QStringList      getMandatoryAttributesKeys(unsigned int pi_uiLevel)  = 0; // must contain all keys for mandatory attributes including minimal entries keys. Keys defines here will be displayed has column name in browser area except for the first one used as id for requesting the source.

    
    
    /* ***********************************************************************/
    /* *************** Get elements data *************************************/
    /* ***********************************************************************/
    virtual QList<levelMinimalEntries>    getMinimalEntries      (unsigned int pi_uiLevel, QString parentKey) = 0; //id ou uid en int ou en QString si  int alors l'implémentation doit avoir une méthode bijective
    virtual QList<QMap<QString, QString>> getMandatoryAttributes (unsigned int pi_uiLevel, QString parentKey) = 0; //must contain all pairs key/value for mandatory attributes excluding minimal entries, except for minimal levelMinimalEntries.key || //must contain all pairs key/value for mandatory attributes including minimal entries. the third first values must match the values returning by getMandatoryMinimalEntries
    virtual bool getAdditionalAttributes(unsigned int pi_uiLevel, QString key, datasetAttributes &po_attributes) = 0;
    
    
    /* ***********************************************************************/
    /* *************** Get data          *************************************/
    /* ***********************************************************************/
    virtual QVariant getDirectData(unsigned int pi_uiLevel, QString key) = 0; //id ou uid en int ou en QString si  int alors l'implémentation doit avoir une méthode bijective
    virtual int      getAssyncData(unsigned int pi_uiLevel, QString key) = 0; //id ou uid en int ou en QString si  int alors l'implémentation doit avoir une méthode bijective. Retourne un id de request
    // virtual QVariant getDataFromRequest(int pi_iRequest) = 0;

    /* ***********************************************************************/
    /* *************** Store data          ***********************************/
    /* ***********************************************************************/
    virtual bool addDirectData(QVariant data,  levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) = 0; //data ->(dataPath, pointer medAbstractData, pointer stream)
    virtual int  addAssyncData(QVariant data,  levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) = 0; //est-ce pertinent de passer un levelMinimalEntries incomplet qui ne sera probablement pas enrichi ?

    virtual bool createPath(QList<levelMinimalEntries> &pio_path,      datasetAttributes const &pi_attributes, unsigned int pi_uiLevel = 0, QString parentKey = "") = 0;
    virtual bool createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey) = 0;

    virtual bool alterMetaData(datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString key) = 0;

    virtual bool getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key) = 0;
    virtual bool setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key) = 0;
    
    virtual bool commitData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) = 0; //data ->(dataPath, pointer medAbstractData, pointer stream)
    virtual int  push(unsigned int pi_uiLevel, QString key) = 0;

    /* ***********************************************************************/
    /* *************** Handle async results   ********************************/
    /* ***********************************************************************/
    virtual QVariant getAsyncResults(int pi_iRequest) = 0; //QVariant may be a path/medAbstractData*/Key string

    /* ***********************************************************************/
    /* *************** Optional functions  ***********************************/
    /* ***********************************************************************/
    inline virtual medAbstractWritingPolicy* getWritingPolicy() { return nullptr; }

signals:
    void progress(int po_iRequest, eRequestStatus status); //getAssyncData, getDataFromRequest, addAssyncData, push
    void connectionStatus(bool);

public slots:
    virtual void abort(int pi_iRequest) = 0;
};


///@{
    /*! @fn bool medAbstractSource::initialization(QString const  &pi_instanceId)
     *  @brief initialization is call by medInria to initialize a new instance.
     *  @details This function must contain all code expected for a complete initialization. After calling of this function this source instance will be consider as operational by medInria.
     *  @param pi_instanceId [in] is the identifier provide by medInria to the new source instance. This value must be store by this instance
     *  @return Must return true if initialization is well done, false in other case.
     */

     /*! @fn bool medAbstractSource::setInstanceName(QString const &pi_instanceName)
      *  @brief Set a human readable name to the instance of source.
      *  @details This function register the humane readable name.
      *  @param pi_instanceName [in] is the name source instance.
      *  @return Must return true if the name registration is well done, false in other case.
      */

      /*! @fn bool medAbstractSource::connect(bool pi_bEnable = true)
       *  @brief Connect or disconnect the source.
       *  @param pi_bEnable [in] is the boolean to indicate if source must be connect (true) or disconnect (false).
       *  @return Must return true if the connection or disconnection is well done, false in other case.
       */
///@}



///@{
/*! @fn bool medAbstractSource::isWritable()
*  @brief Indicates if the source can be write at least at one level.
*  @return True if the is writable, false if it is read only.
*/

/*! @fn bool medAbstractSource::isLocal()
*  @brief Indicates if the source remote via a network or local on same computer of the medInria instance.
*  @return True if the source is local (with very low latency) else false for remote (Hight latency).
*/

/*! @fn bool medAbstractSource::isCached()
*  @brief Indicate if the source plugin handles a system of cache between the source and medInria.
*  @details It is highly recommended to offer a mechanism of cache, with cache write back, only for a remote source.
*  @return True if the source uses a cache mechanism, with cache write back. False otherwise.
*/

/*! @fn bool medAbstractSource::isOnline()
*  @brief Indicates if the source plugin can communicate with the source.
*  @details For a remote source it is the symbol of well establish network communication.<br>
*  @details For a local source it is the symbol of well opening file, for example.
*
*  @details For a source with credentials it is the symbol of a good communication with a good identification.
*  @return True if the source is online. False otherwise.
*/

/*! @fn bool medAbstractSource::isFetchByMinimalEntriesOrMandatoryAttributes()
*  @brief Indicates if the source can be fetch by "minimal entries" or by "mandatory attributes".
*  @return True if the source is fetch by minimalEntries. False otherwise.
*/

/*! @fn bool medAbstractSource::getIOInterface()
*  @brief Indicates if the source can be fetch by "minimal entries" or by "mandatory attributes".
*  @return True if the source is fetch by minimalEntries. False otherwise.
*/
///@}

///@{
/*! @fn QList<medAbstractParameter*> medAbstractSource::getAllParameters()
 *  @brief Provides all the parameters necessary for the operation of the source.
 *  @details This function do not contain filtering parameters.
 *   The returned list can be empty if the source does not need parameters to operate.
 *  @info These parameters will automatically be displayed in the sources settings.
 *  @return a QList that contains all pointers on allocated and pre-initialized parameters.
 */
/*! @fn QList<medAbstractParameter*> medAbstractSource::getCipherParameters()
*  @brief Provides only the parameters necessary for the operation of the source which must be cipher/encrypted inside the file when medInria is switched off.
*  @details This function return parameters already contained in the medAbstractSource::getAllParameters() return.
*  @return a QList that contains all pointers on allocated and pre-initialized cipher parameters.
*/
/*! @fn QList<medAbstractParameter*> medAbstractSource::getVolatileParameters()
*  @brief Provides only the parameters necessary for the operation of the source which must be not saved inside the file when medInria is switched off.
*  @details This function return parameters already contained in the medAbstractSource::getAllParameters() return.
*  All parameters returned must have default value.
*  @return a QList that contains all pointers on allocated and pre-initialized volatile parameters.
*/
/*! @fn QList<medAbstractParameter*> medAbstractSource::getFilteringParameters()
*  @brief Provides parameters to filter the results when a source is fetch.
*  @details For example filtering parameters can be implemented to limit the size of results when the source is fetch.
*  @return a QList that contains all pointers on allocated and pre-initialized filtering parameters.
*/
///@}

/*! @fn QStringList medAbstractSource::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
 *  @brief getMandatoryAttributesKeys returns blablabla1 for the desired level.
 *  @details must contain all keys for mandatory attributes including minimal entries keys. Keys defines here will be displayed has column name in browser area except for the first one used as id for requesting the source.
 *  @param pi_uiLevel [in] is the level to perform this member function valid in the data source.
 *  @return a QStringList of blablabla2.
 */