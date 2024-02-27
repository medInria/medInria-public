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

class medSourceModel;

class medSourceModelItem
{
public:
    medSourceModel                  *model;
    medSourceModelItem                *parentItem;
    QList<medSourceModelItem *>        childItems;
    QMap<int, QMap<int, QVariant> >  itemData; //column, role, value
    QMap<QString, QVariant>          itemMeta;    //keyname, value
    QMap<QString, QString>           itemMetaTag; //keyname, Tag
    int                              iLevel;
    bool                             bCanHaveSubData;

public:
    medSourceModelItem(medSourceModel *model);
    medSourceModelItem(medSourceModelItem *parent);
    ~medSourceModelItem();

    /* ***********************************************************************/
    /* *************** Data manipulation *************************************/
    /* ***********************************************************************/
    int childIndex(QString iid);
    medSourceModelItem* hasChildItemWithIID(QString iid);
    void setParent(medSourceModelItem *parent);
    QVariant data  (int column,  int role = Qt::DisplayRole) const;
    void setMetaData(QMap<QString, QString> const &attributes, QMap<QString, QString> const &tags);


    inline void setData(QVariant value, int column = 0, int role = Qt::DisplayRole) { itemData[column][role] = value; }
    inline QString iid() {return itemData[0][0].toString(); }
    inline medSourceModelItem *child(int row) const { return childItems.value(row); };
    inline void append(medSourceModelItem *child) {childItems.append(child);}
    inline void insert(int position, medSourceModelItem *child) { childItems.insert(position, child); }
    QString iid(QString displayValue);

    bool containRoleValues(QMap<int /*role*/, QVariantList /*values*/> mapList);


    /* ***********************************************************************/
    /* *************** Data location functions *******************************/
    /* ***********************************************************************/
    inline medSourceModelItem *parent() const { return parentItem; };
    inline int childCount() const { return childItems.size(); };
    inline int level() const { return iLevel; };
    inline bool canHaveSubData() { return bCanHaveSubData; }

    int row() const;
    bool removeRows(int row, int count);

    QList<medSourceModelItem *> offspringList();


    /* ***********************************************************************/
    /* *************** Data conversion ***************************************/
    /* ***********************************************************************/
    QStringList relativeUri();
    QStringList uri();
    QString     uriAsString();
    QModelIndex index();
};
