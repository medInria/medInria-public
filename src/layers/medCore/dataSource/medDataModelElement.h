#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QAbstractItemModel>

#include <medDataModelItem.h>

#include <medCoreExport.h>

class medDataModel;
struct medDataModelElementPrivate;

class MEDCORE_EXPORT medDataModelElement : public QAbstractItemModel
{

    Q_OBJECT

public:	
    medDataModelElement(medDataModel *parent, QString const & sourceIntanceId);
    virtual ~medDataModelElement();

    // ////////////////////////////////////////////////////////////////////////
    // Pure Virtual Override
    QVariant	data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex	parent(const QModelIndex &index) const override;

    int	columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int	rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // ////////////////////////////////////////////////////////////////////////
    // Simple Virtual Override
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;


    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;


    medDataModelItem* getItem(const QModelIndex &index) const;
    void setColumnAttributes(int p_iLevel, QStringList &attributes);

public slots:
    void itemPressed(QModelIndex const &index);



private:
    bool getColumnNames(int const &m_iLevel) const;
    int  getLevelColumCount(int pi_iLevel) const;
    void populateLevel(QModelIndex const &index, QString const &key);
    //void populateLevel(medDataModelItem * pItemCurrent, QString const &key, QModelIndex const &index, bool subLevel = false);
    bool currentLevelFetchable(medDataModelItem * pItemCurrent);


signals:


private:
    medDataModelElementPrivate* d;


};