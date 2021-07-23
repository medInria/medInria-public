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

class medDataIndex;

class medDataModelItem
{

public:
    medDataModelItem();
    ~medDataModelItem();

    /* ***********************************************************************/
    /* *************** Data manipulation *************************************/
    /* ***********************************************************************/





    /* ***********************************************************************/
    /* *************** Data location functions *******************************/
    /* ***********************************************************************/
    int childCount();







    // medAbstractDatabaseItem *child(int row) = 0;
    // medAbstractDatabaseItem *parent() = 0;
    // 
    // void setParent(medAbstractDatabaseItem *parent) = 0;
    // 
    // void append(medAbstractDatabaseItem *child) = 0;
    // 
    // int row() const = 0;
    // int childCount() const = 0;
    // int childNumber() const = 0;
    // int columnCount() const = 0;
    // 
    // QVariant data(int column) const = 0;
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
