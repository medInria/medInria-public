#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2021. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QVariant>

//class medDataModelItemPrivate;
class medSourceItemModel;

class medDataModelItem
{
public:
    medSourceItemModel              *m_model;
    medDataModelItem                *m_parentItem;
    QList<medDataModelItem *>        m_childItems;
    QMap<int, QMap<int, QVariant> >  m_itemData; //column, role, value
    QMap<QString, QVariant>          m_itemMeta;    //keyname, value
    QMap<QString, QString>           m_itemMetaTag; //keyname, Tag
    int                              m_iLevel;
    bool                             m_bCanHaveSubData;

public:
    medDataModelItem(medSourceItemModel *model);
    medDataModelItem(medDataModelItem *parent);
    ~medDataModelItem();

    /* ***********************************************************************/
    /* *************** Data manipulation *************************************/
    /* ***********************************************************************/
    void setData(QVariant value, int column = 0, int role = Qt::DisplayRole);
    QVariant data(int section, int role = Qt::DisplayRole) const;
    QVariant metaData(int column, int role = Qt::DisplayRole) const;
    //QVariant metaData(int column, int role = Qt::DisplayRole) const;//TODO19
    inline QString iid() {return m_itemData[0][0].toString(); }

    int childIndex(QString iid);
    medDataModelItem* hasChildItemWithIID(QString iid);


    medDataModelItem *child(int row) const;
    void setParent(medDataModelItem *parent);
    void append(medDataModelItem *child);
    void insert(int position, medDataModelItem *child);




    /* ***********************************************************************/
    /* *************** Data location functions *******************************/
    /* ***********************************************************************/
    int childCount() const;
    int level() const;
    int row() const;
    medDataModelItem *parent() const;

    //bool insertChildren(int position, int count);
    bool removeRows(int row, int count);

    bool canHaveSubData();

    QString uri();
};
