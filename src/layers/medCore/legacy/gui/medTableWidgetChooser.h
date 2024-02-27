#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QTableWidget>
#include <QTableWidgetItem>
#include <medImageViewEnum.h>

#include <medCoreExport.h>

class medTableWidgetChooserPrivate;
class MEDCORE_EXPORT medTableWidgetChooser : public QTableWidget
{
    Q_OBJECT

public:
     medTableWidgetChooser(QWidget *parent = nullptr, int rowCount = 5, int colCount = 5, int sideSize = 30);
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

class MEDCORE_EXPORT medTableWidgetItem : public QTableWidgetItem
{
public:
    medTableWidgetItem(const QIcon &icon, const QString &text, int type = Type):QTableWidgetItem(icon, text, type){}
    medImageView::Orientation orientation();

    enum ItemType { AxialType = UserType, CoronalType, SagittalType, ThreeDimType};
};
