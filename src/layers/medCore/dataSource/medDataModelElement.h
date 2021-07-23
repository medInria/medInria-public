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

//#include <medDataModel.h>

#include <medCoreExport.h>

class medDataModel;
class MEDCORE_EXPORT medDataModelElement : public QAbstractItemModel
{

    Q_OBJECT

public:	
    medDataModelElement(medDataModel *parent, QString const & sourceIntanceId);
    virtual ~medDataModelElement();

    QVariant	data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QModelIndex	index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex	parent(const QModelIndex &index) const override;

    int	columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int	rowCount(const QModelIndex &parent = QModelIndex()) const override;



    void setColumnAttributes(unsigned int p_uiLevel, QStringList &attributes);

public slots:
    void itemPressed(QModelIndex const &index);

private:


signals:


private:
    medDataModel *m_parent;
    QString m_SourceInstanceId;
    QStringList m_columnName;
    bool m_bOnline;
    bool m_bWritable;
    bool m_bCache;
    //medDataModelItem m_root;
    int m_iRow;
    int m_iCol;


};