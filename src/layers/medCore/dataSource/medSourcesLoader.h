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


class MEDCORE_EXPORT medDBSourcesLoader : public QObject
{

    Q_OBJECT
    using instantiateSource = medAbstractSource * (*)();

public:	
    static medDBSourcesLoader* instance();
    ~medDBSourcesLoader();
	
    /* ***********************************************************************/
    /* *************** Add a source type ********************************/
    /* ***********************************************************************/
	bool                                       registerSourceType(QString type, QString name, QString description, instantiateSource instanciator);
	QList<std::tuple<QString,QString,QString>> sourcesTypeAvailables();
    
    /* ***********************************************************************/
    /* *************** Create/remove sources *********************************/
    /* ***********************************************************************/
	bool createCnx(QString &instanceId, QString const &type); //Provide instanceId like an internal name and the type of source. instanceId can be altered to avoid duplicates. If instanceId is empty the function will generate one
	bool removeCnx(QString const &instanceId);	
	
    /* ***********************************************************************/
    /* *************** Handle sources ****************************************/
    /* ***********************************************************************/
	QList<medAbstractSource*> sourcesList();
	medAbstractSource*        getSource(QString const & instanceId);
    bool                      renameSource(QString const & instanceId, QString const & name);
	
	bool loadFromDisk(); //call after each add or remove source instance, each call to the destructor. Must be crytographied
private:
    medDBSourcesLoader();

    bool saveToDisk();   //call after each add or remove source instance, each call to the destructor. Must be crytographied
    

    void reloadCnx(QJsonObject &obj);

    medAbstractSource* createInstanceOfSource(QString const & type) const;
    bool generateUniqueSourceId(QString & Id, QString const & type) const;
    void reparseUnresolvedCnx();

    static void convertCipherParamToJson(QJsonObject & po_oJson, medAbstractParameter *pi_pParam);
	
    QString m_CnxParametersPath;

signals:
    void sourceAdded(medAbstractSource*);  // Signal to indicate a new source
	void sourceRemoved(medAbstractSource*); // Signal to indicate a source was removed
	
	void connectorNotAvailable(QString);   // Error a DBSource configuration exist without associated medSource plugin


private:

    struct medSourceTool
    {
        QString type;                   //Unique id for a type of medSource (QString ou int ou typeid)
        QString name;                   //Human readable name
        QString description;            //Human readable detailed information
        instantiateSource instanciator; //Function pointer to instantiate a connection to the source object
    };

    QMutex m_mutexMap;
    QMap<QString, QString > m_instanceMapType; //instance, instanceId
    QMap<QString, QSharedPointer<medAbstractSource> > m_instancesMap; //instanceId, instance
    QMap<QString, medSourceTool>      m_sourcesMap;  //Source connector type name, function pointer to instantiate a connection to the source object
    QList<QJsonObject> m_unresolvedSavedCnx;
    
    
    static medDBSourcesLoader *s_instance;
};