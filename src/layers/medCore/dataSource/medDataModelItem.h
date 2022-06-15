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
    medSourceItemModel              *model;
    medDataModelItem                *parentItem;
    QList<medDataModelItem *>        childItems;
    QMap<int, QMap<int, QVariant> >  itemData; //column, role, value
    QMap<QString, QVariant>          itemMeta;    //keyname, value
    QMap<QString, QString>           itemMetaTag; //keyname, Tag
    int                              iLevel;
    bool                             bCanHaveSubData;

public:
    medDataModelItem(medSourceItemModel *model);
    medDataModelItem(medDataModelItem *parent);
    ~medDataModelItem();

    /* ***********************************************************************/
    /* *************** Data manipulation *************************************/
    /* ***********************************************************************/
    int childIndex(QString iid);
    medDataModelItem* hasChildItemWithIID(QString iid);
    void setParent(medDataModelItem *parent);
    QVariant data  (int column,  int role = Qt::DisplayRole) const;
    void setMetaData(QMap<QString, QVariant> const &attributes, QMap<QString, QString> const &tags);


    inline void setData(QVariant value, int column = 0, int role = Qt::DisplayRole) { itemData[column][role] = value; }
    inline QString iid() {return itemData[0][0].toString(); }
    inline medDataModelItem *child(int row) const { return childItems.value(row); };
    inline void append(medDataModelItem *child) {childItems.append(child);}
    inline void insert(int position, medDataModelItem *child) { childItems.insert(position, child); }
    QString iid(QString displayValue);

    bool isAssociatedAbstractData();


    /* ***********************************************************************/
    /* *************** Data location functions *******************************/
    /* ***********************************************************************/
    inline medDataModelItem *parent() const { return parentItem; };
    inline int childCount() const { return childItems.size(); };
    inline int level() const { return iLevel; };
    inline bool canHaveSubData() { return bCanHaveSubData; }

    int row() const;
    bool removeRows(int row, int count);

    QList<medDataModelItem *> offspringList();


    /* ***********************************************************************/
    /* *************** Data conversion ***************************************/
    /* ***********************************************************************/
    QStringList uriAsList();
    QString     uriAsString();
    QModelIndex index();
};
