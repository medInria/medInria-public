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
#include <medAbstractData.h>
#include <medAbstractParameter.h>

#include <medCoreExport.h>

class MEDCORE_EXPORT medAbstractSource : public QObject
{
    Q_OBJECT

public:
    /* ********  Julien tu peux choisir ou proposer d'autres representation ***********************************************/
    /* */   using  datasetAttributes1 = QMap<QString, QPair<QVariant, QString> >; // <keyName , <value, tag>>
    /* */   using  datasetAttributes2 = QMap<QString, QMap <QString, QVariant> >; // <keyName , <dataTypeName, value>, dataTypeName can be "value", "role", tag", ...
    /* */   
    /* */   struct datasetAttributes3_sub
    /* */   {
    /* */       QString                 value;  //value
    /* */       QMap<QString, QVariant> option; //optional <dataTypeName, value>, dataTypeName can be "value", "role", tag", ...
    /* */   };
    /* */   using datasetAttributes3 = QMap<QString, datasetAttributes3_sub >; // <keyName , datasetAttributes3> equivalent to datasetAttributes2
    /* */   
    /* */   struct datasetAttributes4
    /* */   {
    /* */       QMap<QString, QString> values; // <keyName, value>
    /* */       QMap<QString, QString> tags;   // <keyName, tag value>
    /* */   };
    /* */   
    /* */   using  listAttributes1 = QList<datasetAttributes1>;
    /* */   using  listAttributes2 = QList<datasetAttributes2>;
    /* */   using  listAttributes3 = QList<datasetAttributes3>;
    /* */   using  listAttributes4 = QList<datasetAttributes4>;
    /* -------------------------------------------------------------------------------------------------------------------*/

    /**
     * @brief This structure represents the minimal data useful to build the tree. Minimal entries are a sub-part of mandatories attributes.
     */
    struct levelMinimalEntries
    {
        QString key; // TODO : change
        QString name;
        QString description;
    };

    struct levelMinimalKeys
    {
        QString key;
        QString name;
        QString description;
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
    virtual QList<medAbstractParameter*> getVolatilParameters() = 0;

    /* ***********************************************************************/
    /* *************** Get source properties *********************************/
    /* ***********************************************************************/
    virtual bool isWriteable() = 0;
    virtual bool isLocal()     = 0;
    virtual bool isCached()    = 0;
    virtual bool isOnline()    = 0;
    virtual bool isFetchByMinimalEntriesOrMandatoryAttributes() = 0; //true: minimalEntries, false: MandatoryAttributes  
    
    /* ***********************************************************************/
    /* *************** Get source structure information **********************/
    /* ***********************************************************************/
    virtual QString          getInstanceName() = 0;
    virtual QString          getInstanceId()   = 0;
                             
    virtual unsigned int     getLevelCount() = 0;
    virtual QStringList      getLevelNames() = 0;
    virtual QString          getLevelName(unsigned int pi_uiLevel) = 0;

    //virtual void             getMinimalKeys(unsigned int pi_uiLevel, QString &key, QString &name, QString &description) = 0;
    //virtual levelMinimalKeys getMinimalKeys(unsigned int pi_uiLevel) = 0;
    virtual QStringList      getMandatoryAttributesKeys(unsigned int pi_uiLevel)  = 0; // must contain all keys for mandatory attributes including minimal entries keys. Keys defines here will be displayed has column name in browser area except for the first one used as id for requesting the source.

    
    
    /* ***********************************************************************/
    /* *************** Get elements data *************************************/
    /* ***********************************************************************/
    virtual QList<levelMinimalEntries>    getMinimalEntries      (unsigned int pi_uiLevel, QString parentKey) = 0; //id ou uid en int ou en QString si  int alors l'implémentation doit avoir une méthode bijective
    virtual QList<QMap<QString, QString>> getMandatoryAttributes (unsigned int pi_uiLevel, QString parentKey) = 0; //must contain all pairs key/value for mandatory attributes excluding minimal entries, except for minimal levelMinimalEntries.key || //must contain all pairs key/value for mandatory attributes including minimal entries. the third first values must match the values returning by getMandatoryMinimalEntries
    virtual bool getAdditionalAttributes(unsigned int pi_uiLevel, QString key, datasetAttributes4 &po_attributes) = 0;
    
    
    /* ***********************************************************************/
    /* *************** Get data          *************************************/
    /* ***********************************************************************/
    virtual QVariant getDirectData(unsigned int pi_uiLevel, QString key) = 0; //id ou uid en int ou en QString si  int alors l'implémentation doit avoir une méthode bijective
    virtual int     getAssyncData(unsigned int pi_uiLevel, QString key) = 0; //id ou uid en int ou en QString si  int alors l'implémentation doit avoir une méthode bijective. Retourne un id de request
    
    /* ***********************************************************************/
    /* *************** Store data          ***********************************/
    /* ***********************************************************************/
    //TODO: store a dataset
    virtual QString addData(QVariant data, QString parentUri, QMap<QString, QString> mandatoryAttributes, datasetAttributes4 additionalAttributes) = 0;
    //TODO: alter metaData
    //TODO: store complementaries data like thumbnail
    
    
signals:
    void progress(int po_iRequest, eRequestStatus status);
    void connectionStatus(bool);

public slots:
    virtual void abort(int pi_iRequest) = 0;
};

/*! @fn QStringList medAbstractSource::getMandatoryAttributesKeys(unsigned int pi_uiLevel)
 *  @brief getMandatoryAttributesKeys returns blablabla1 for the desired level.
 *  @details must contain all keys for mandatory attributes including minimal entries keys. Keys defines here will be displayed has column name in browser area except for the first one used as id for requesting the source.
 *  @param pi_uiLevel [in] is the level to perform this member function valid in the data source.
 *  @return a QStringList of blablabla2.
 */