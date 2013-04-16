/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QItemDelegate>
#include <QModelIndex>
#include <VectorWidget.h>

class DiffusionSequenceDelegate: public QItemDelegate {
public:

    DiffusionSequenceDelegate(QObject* parent=0): QItemDelegate(parent) { }

    QWidget* createEditor(QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index) const {
        if (index.column()==0)
            return 0;
        return new VectorWidget(3,parent);
    }

    void setEditorData(QWidget* editor,const QModelIndex& index) const {
        const QStringList& values = index.model()->data(index,Qt::EditRole).toString().split(",");
        VectorWidget* widget = static_cast<VectorWidget*>(editor);
        for (unsigned i=0;i<3;++i)
            widget->value(i+1) = values.at(i).simplified().toDouble();
    }

    void setModelData(QWidget* editor,QAbstractItemModel* model,const QModelIndex& index) const {
        const VectorWidget* widget = static_cast<VectorWidget*>(editor);
        const QString& value = QString("%1, %2, %3").arg(widget->value(1)).arg(widget->value(2)).arg(widget->value(3));
        model->setData(index,value,Qt::EditRole);
    }

    void updateEditorGeometry(QWidget* editor,const QStyleOptionViewItem& option,const QModelIndex&) const {
        editor->setGeometry(option.rect);
    }
};


