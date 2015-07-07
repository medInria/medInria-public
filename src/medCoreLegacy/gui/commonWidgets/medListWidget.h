/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <QtGui>
#include <QDebug>

/**
  * QListWidget doesn't seem to be able to resize itself to its content
  * medListWidget should do so.
  */
class MEDCORE_EXPORT medListWidget : public QListWidget
{
public:
    medListWidget(QWidget *parent = 0):QListWidget(parent)
    {}

    virtual QSize sizeHint() const
    {
        int height = 0;
        for(int i=0; i< this->count(); i++)
        {
           QListWidgetItem* item = this->item(i);
           if(this->itemWidget(item))
             height += this->itemWidget(item)->height();
           else height += QListWidget::sizeHintForRow(i);
        }
        height += QListWidget::sizeHintForRow(0);

        if(this->count() == 0)
            return QListWidget::sizeHint();
        else return QSize(QListWidget::sizeHintForColumn(0), height);
    }
};
