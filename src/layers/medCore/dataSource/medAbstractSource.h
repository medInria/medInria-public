#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>

#include <medAbstractData>

class MEDSOURCE_EXPORT medAbstractSource : public QObject
{

    QOBJECT

public:	
    medAbstractSource();
    virtual ~medAbstractSource() = 0;

    QOBJECT

public:	
    medAbstractSource();
    virtual ~medAbstractSource() = 0;
    
    struct levelMinimalEntries
    {
        QString id;
        QString name;
        QString description;
    };
	
	enum eRequestStatus : int
	{
		aborted = -3,
		cnxLost = -2,
		faild   = -1,
		finish  =  0,
		pending =  1
	}
    
    /* ***********************************************************************/
    /* *************** Get source properties *********************************/
    /* ***********************************************************************/
    virtual bool isWriteable() = 0;
    virtual bool isLocal() = 0;
    virtual bool isCached() = 0;
    virtual bool isOnline() = 0;
    
    
    /* ***********************************************************************/
    /* *************** Get source structure information **********************/
    /* ***********************************************************************/
    QString      getDBName();
    QString      getDBId();
    
    unsigned int getLevelCount();
    QStringList  getLevelNames();
    QString      getLevelNane(unsigned int pi_uiLevel);
    
    QStringList  getMandatoryAttributesKeys(unsigned int pi_uiLevel);
    QStringList  getAdditionalAttributesKeys(unsigned int pi_uiLevel);
    
    
    /* ***********************************************************************/
    /* *************** Get elements data *************************************/
    /* ***********************************************************************/
    QList<levelMinimalEntries>    getMinimalEntries(unsigned int pi_uiLevel, QString id); //id ou uid en int ou en QString si  int alors l'implémentation doit avoir une méthode bijective
    QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, int id); //ou QVarient
    QList<QMap<QString, QString>> getAdditionalAttributes(unsigned int pi_uiLevel, int id); //ou QVarient
    
    
    /* ***********************************************************************/
    /* *************** Get data          *************************************/
    /* ***********************************************************************/
    medAbstractData* getDirectData(unsigned int pi_uiLevel, QString id); //id ou uid en int ou en QString si  int alors l'implémentation doit avoir une méthode bijective
    int              getAssyncData(unsigned int pi_uiLevel, QString id); //id ou uid en int ou en QString si  int alors l'implémentation doit avoir une méthode bijective. Retourne un id de request
    
    /* ***********************************************************************/
    /* *************** Store data          ***********************************/
    /* ***********************************************************************/
    //TODO store a dataset
    //TODO alter metaData
    //TODO stroe complementaries data like thumbnail
    
    
signal:
    void progress(int po_iRequest, eRequestStatus status);

public slots:
    void abort(int pi_iRequest);
}

MEDSOURCE_IMPORT medAbstractSource *instanciateSource();
MEDSOURCE_IMPORT void instanciateSource(medAbstractSource *);
using instanciateSource = medAbstractSource*();