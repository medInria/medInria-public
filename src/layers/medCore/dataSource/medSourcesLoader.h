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
#include <tuple>

class QStandardPaths;
class QCryptographicHash;
class QPasswordDigestor;
class QJsonObject;
class QJsonDocument;


class medDBSourcesLoader : public QObject
{

    QOBJECT

public:	
    medDBSourcesLoader();
    ~medDBSourcesLoader();
	
    /* ***********************************************************************/
    /* *************** Add a source type ********************************/
    /* ***********************************************************************/
	bool                                       registerSourceType(QString sourceType, instanciateSource instanciator);
	QList<std::tuple<QString,QString,QString>> sourcesTypeAvailables();
    
    /* ***********************************************************************/
    /* *************** Create/remove sources *********************************/
    /* ***********************************************************************/
	bool createNewCnx(QString &IdName, QString const &sourceType); //Provide IdName like a nane and the type of source. IdName can be altered to avoid duplicates
	bool removeOldCnx(QString &IdName);	
	
    /* ***********************************************************************/
    /* *************** Get sources *******************************************/
    /* ***********************************************************************/
	QList<medAbstractSource*> sourcesList();
	medAbstractSource*        getSource(QString IdName); 
	
	
private:
    bool saveToDisk();   //call after each add or remove source instance, each call to the destructor. Must be crytographied
    bool loadFromDisk(); //call after each add or remove source instance, each call to the destructor. Must be crytographied
	
signals:
    void sourceAdded(medAbstractSource*);  // Signal to indiacte a new source
	void sourceRemove(medAbstractSource*); // Signal to indicate a source was removed
	
	void connectorNotAvailable(QString);   // Error a DBSource configuration exist without associated medSource plugin


private:

    struct
	{
		QString id;                     //Unique id for a type of medSource (QString ou int ou typeid)
		QString name;                   //Human readable name
		QString Description;            //Human readable detailed information
		instanciateSource instanciator; //Function pointer to instanciate a connection to the source object
	}medSrouceTool;
	
    QMap<QString, medAbstractSource*> dbSourcesInstances; //IdName, instance
    QMap<QString, medSrouceTool>      dbSourcesModelMap;  //Db connector type name, function pointer to instanciate a connection to the source object
	
}