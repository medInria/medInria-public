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

class medDataModelItemPrivate;

class medDataModelItem
{

public:
    medDataModelItem();
    ~medDataModelItem();

    /* ***********************************************************************/
    /* *************** Data manipulation *************************************/
    /* ***********************************************************************/
    QVariant data(int column) const;
    void setData(QStringList const& value);

    medDataModelItem *child(int row) const;
    void setParent(medDataModelItem *parent);
    void append(medDataModelItem *child);




    /* ***********************************************************************/
    /* *************** Data location functions *******************************/
    /* ***********************************************************************/
    int childCount() const;
    unsigned int level() const;
    int row() const;
    medDataModelItem *parent() const;

private:
    medDataModelItemPrivate *d;






    // int childNumber() const = 0;
    // int columnCount() const = 0;
   
    // 
    // bool insertChildren(const medDataIndex& index, int position, int count, int columns) = 0;
    // bool insertColumns(int position, int columns) = 0;
    // 
    // bool removeChildren(int position, int count, bool deleteChildren = true) = 0;
    // 
    // bool removeColumns(int position, int columns) = 0;
    // 
    // bool setData(int column, const QVariant& value) = 0;
    // 
    // const medDataIndex & dataIndex() const = 0;
    // void setDataIndex(const medDataIndex &) = 0;
    // 
    // QVariant attribute(int column) = 0;
    // QVariant value(int column) = 0;
    // 
    // QList<QVariant> attributes() = 0;
    // QList<QVariant> values() = 0;
    // 
    // int rowOf(medAbstractDatabaseItem *child) const = 0;
};
