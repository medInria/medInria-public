/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QTableWidget>
#include <QTableWidgetItem>
#include <medCoreLegacyExport.h>
#include <medImageViewEnum.h>
class medTableWidgetChooserPrivate;

class MEDCORELEGACY_EXPORT medTableWidgetChooser : public QTableWidget
{
    Q_OBJECT

public:
     medTableWidgetChooser(QWidget *parent = 0, int rowCount = 5, int colCount = 5, int sideSize = 30);
    ~medTableWidgetChooser();

    QSize sizeHint() const;
    int sizeHintForRow(int row) const;
    int sizeHintForColumn(int column) const;

    void setSideSize(int size);
signals:
    void selected(unsigned int rows, unsigned int cols);

protected:
    void mouseReleaseEvent(QMouseEvent *event);

private:
    medTableWidgetChooserPrivate *d;
};


class MEDCORELEGACY_EXPORT medTableWidgetItem : public QTableWidgetItem
{
public:
    medTableWidgetItem(const QIcon &icon, const QString &text, int type = Type):QTableWidgetItem(icon, text, type){}
    medImageView::Orientation orientation();

    enum ItemType { AxialType = UserType, CoronalType, SagittalType, ThreeDimType};
};
