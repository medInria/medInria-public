/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QVariant>
#include <medCoreExport.h>

class medDataIndex;

class MEDCORE_EXPORT medAbstractDatabaseItem
{

public:
    virtual ~medAbstractDatabaseItem(){}//we need a virtual destructor here if we want the children to be deleted in the implementation of this class.

    virtual medAbstractDatabaseItem *child(int row) = 0;
    virtual medAbstractDatabaseItem *parent() = 0;

    virtual void setParent(medAbstractDatabaseItem *parent) = 0;

    virtual void append(medAbstractDatabaseItem *child) = 0;

    virtual int row() const = 0;
    virtual int childCount() const = 0;
    virtual int childNumber() const = 0;
    virtual int columnCount() const = 0;

    virtual QVariant data(int column) const = 0;

    virtual bool insertChildren(const medDataIndex& index, int position, int count, int columns)= 0;
    virtual bool insertColumns(int position, int columns)= 0;

    virtual bool removeChildren(int position, int count, bool deleteChildren = true) = 0;

    virtual bool removeColumns(int position, int columns) = 0;

    virtual bool setData(int column, const QVariant& value) = 0;

    virtual const medDataIndex & dataIndex () const = 0;
    virtual void setDataIndex (const medDataIndex &) = 0;
	
    virtual QVariant attribute(int column) = 0;
    virtual QVariant value(int column) = 0;
    
    virtual QList<QVariant> attributes() = 0;
    virtual QList<QVariant> values() = 0;
    
    virtual int rowOf(medAbstractDatabaseItem *child) const = 0;

};

