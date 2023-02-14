#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSource.h>
#include <medCoreExport.h>
#include <tuple>

class QStandardPaths;
class QCryptographicHash;
class QPasswordDigestor;
class QJsonObject;
class QJsonDocument;
class QMutex;
template <class T> class QSharedPointer;

#ifndef ED_DATASOURCES_FILENAME
#define MED_DATASOURCES_FILENAME "DataSources.json"
#endif // !ED_DATASOURCES_FILENAME


class MEDCORE_EXPORT medSourcesLoader : public QObject
{

    Q_OBJECT
    using instantiateSource = medAbstractSource * (*)();

public:	
    static medSourcesLoader* instance(QObject *parent = nullptr);
    ~medSourcesLoader();
	
    /* ***********************************************************************/
    /* *************** Add a source type *************************************/
    /* ***********************************************************************/
	bool registerSourceType(QString type, QString name, QString description, instantiateSource instanciator);
	QList<std::tuple<QString,QString,QString>> sourcesTypeAvailables();
    
    /* ***********************************************************************/
    /* *************** Create/remove sources *********************************/
    /* ***********************************************************************/
	bool createCnx(QString &instanceId, QString const &type); //Provide instanceId like an internal name and the type of source. instanceId can be altered to avoid duplicates. If instanceId is empty the function will generate one
	bool removeCnx(QString const &instanceId);
	
    /* ***********************************************************************/
    /* *************** Handle sources ****************************************/
    /* ***********************************************************************/
    QStringList sourcesIdList() { return m_sourcesIDs; }
	QList<medAbstractSource*> sourcesList();
	medAbstractSource*        getSource(QString const & instanceId);
    medAbstractSource*        getDefaultWorkingSource();
    bool                      setDefaultWorkingSource(QString const &instanceId);
    bool                      renameSource(QString const & instanceId, QString const & name);
	
	bool loadFromDisk(); //call after each add or remove source instance, each call to the destructor. Must be encrypted

    bool setPath(QString path);
    QString getPath();

public slots:
    void changeSourceOrder(int oldPlace, int newPlace);

private:
    medSourcesLoader(QObject *parent = nullptr);

    bool saveToDisk() const;   //call after each add or remove source instance, each call to the destructor. Must be encrypted
    

    void reloadCnx(QJsonObject &obj);

    medAbstractSource* createInstanceOfSource(QString const & type) const;
    bool generateUniqueSourceId(QString & Id, QString const & type) const;
    void reparseUnresolvedCnx();

    static void convertCipherParamToJson(QJsonObject & po_oJson, medAbstractParameter *pi_pParam);
	
    QString m_CnxParametersPath;
    QString m_CnxParametersFile;

signals:
    void sourceAdded(medAbstractSource*);  // Signal to indicate a new source
	void sourceRemoved(medAbstractSource*); // Signal to indicate a source was removed
	
	void connectorNotAvailable(QString);   // Error a Source configuration exist without associated medSource plugin
    void sourcesUpdated();

    void defaultWorkingSource(medAbstractSource*);

private:

    struct medSourceTool
    {
        QString type;                   //Unique id for a type of medSource (QString ou int ou typeid)
        QString name;                   //Human readable name
        QString description;            //Human readable detailed information
        instantiateSource instanciator; //Function pointer to instantiate a connection to the source object
    };

    QMutex m_mutexMap;
    QStringList m_sourcesIDs;
    QMap<QString, QString > m_instanceMapType; //instance, instanceId
    QMap<QString, QSharedPointer<medAbstractSource> > m_instancesMap; //instanceId, instance
    QMap<QString, medSourceTool>      m_sourcesMap;  //Source connector type name, function pointer to instantiate a connection to the source object
    QList<QJsonObject> m_unresolvedSavedCnx;
    medAbstractSource * m_defaultSource;
    
    
    static medSourcesLoader *s_instance;
};