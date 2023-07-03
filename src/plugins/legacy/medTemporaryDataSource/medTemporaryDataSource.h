/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#pragma once

#include "medTemporaryDataSourcePluginExport.h"

#include <medAbstractSource.h>

struct medTemporaryDataSourcePrivate;

class medTemporaryDataSource : public medAbstractSource
{

public:
    explicit medTemporaryDataSource();
    ~medTemporaryDataSource() override;

    /* ***********************************************************************/
    /* *************** Init/set/ctrl source properties ***********************/
    /* ***********************************************************************/
    bool initialization(const QString &pi_instanceId) override;

    bool setInstanceName(const QString &pi_instanceName) override;

    bool connect(bool pi_bEnable) override;

    QList<medAbstractParameter *> getAllParameters() override;

    QList<medAbstractParameter *> getCipherParameters() override;

    QList<medAbstractParameter *> getVolatileParameters() override;

    QList<medAbstractParameter *> getFilteringParameters() override;

    /* ***********************************************************************/
    /* *************** Get source properties *********************************/
    /* ***********************************************************************/
    bool isWritable() override;

    bool isLocal() override;

    bool isCached() override;

    bool isOnline() override;

    bool isFetchByMinimalEntriesOrMandatoryAttributes() override;

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
    QList<levelMinimalEntries> getMinimalEntries(unsigned int pi_uiLevel, QString parentId) override;

    QList<QMap<QString, QString>> getMandatoryAttributes(unsigned int pi_uiLevel, QString parentId) override;

    bool getAdditionalAttributes(unsigned int pi_uiLevel, QString id, datasetAttributes &po_attributes) override;

    /* ***********************************************************************/
    /* *************** Get data          *************************************/
    /* ***********************************************************************/
    QVariant getDirectData(unsigned int pi_uiLevel, QString key) override;

    int getAssyncData(unsigned int pi_uiLevel, QString id) override;

    virtual int getIOInterface() override;
    virtual QMap<QString, QStringList> getTypeAndFormat() override;
    virtual bool addDirectData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;
    virtual int addAssyncData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;
    virtual QVariant getAsyncResults(int pi_iRequest) override;
    virtual bool createPath(QList<levelMinimalEntries> &pio_path, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel = 0, QString parentKey = "") override;
    virtual bool createFolder(levelMinimalEntries &pio_minimalEntries, datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString parentKey) override;
    virtual bool alterMetaData(datasetAttributes const &pi_attributes, unsigned int pi_uiLevel, QString key) override;
    virtual bool getThumbnail(QPixmap &po_thumbnail, unsigned int pi_uiLevel, QString key) override;
    virtual bool setThumbnail(QPixmap &pi_thumbnail, unsigned int pi_uiLevel, QString key) override;
    virtual bool commitData(QVariant data, levelMinimalEntries &pio_minimalEntries, unsigned int pi_uiLevel, QString parentKey) override;
    virtual int push(unsigned int pi_uiLevel, QString key) override;

public slots:
    void abort(int pi_iRequest) override;

private:
    // methods

private:
    // members
    medTemporaryDataSourcePrivate *d;
};
