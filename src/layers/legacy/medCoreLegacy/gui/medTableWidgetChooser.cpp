/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTableWidgetChooser.h>

#include <QtGui>
#include <QtWidgets>

class medTableWidgetChooserPrivate
{
public:
    unsigned int left;
    unsigned int right;
    unsigned int top;
    unsigned int bottom;
    int sideSize;
};

medTableWidgetChooser::medTableWidgetChooser(QWidget *parent, int rowCount, int colCount, int sideSize) : QTableWidget(rowCount, colCount, parent), d(new medTableWidgetChooserPrivate)
{
    d->sideSize = sideSize;
    this->resizeColumnsToContents();
    this->resizeRowsToContents();

    this->horizontalHeader()->setHidden(true);
    this->verticalHeader()->setHidden(true);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setFrameShape(QFrame::NoFrame);

    d->left = d->right = d->top = d->bottom = 0;

    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

}

medTableWidgetChooser::~medTableWidgetChooser(void)
{
    delete d;
    d = NULL;
}

QSize medTableWidgetChooser::sizeHint(void) const
{
    return QSize(this->columnCount()*d->sideSize, this->rowCount()*d->sideSize);
}

int medTableWidgetChooser::sizeHintForRow(int row) const
{
    Q_UNUSED(row);
    return d->sideSize;
}

int medTableWidgetChooser::sizeHintForColumn(int column) const
{
    Q_UNUSED(column);
    return d->sideSize;
}


void medTableWidgetChooser::mouseReleaseEvent(QMouseEvent *event)
{

    QItemSelection selec = this->selectionModel()->selection();
    d->left   = qMin(d->left,   static_cast<unsigned int>(selec.first().left()));
    d->right  = qMax(d->right,  static_cast<unsigned int>(selec.first().right()));
    d->top    = qMin(d->top,    static_cast<unsigned int>(selec.first().top()));
    d->bottom = qMax(d->bottom, static_cast<unsigned int>(selec.first().bottom()));

    emit selected(d->bottom - d->top + 1, d->right - d->left + 1);

    QTableWidget::mouseReleaseEvent(event);

    this->clearSelection();

    d->left   = 0;
    d->right  = 0;
    d->top    = 0;
    d->bottom = 0;
}


medImageView::Orientation medTableWidgetItem::orientation()
{
    switch (type())
    {
    case medTableWidgetItem::AxialType:
        return medImageView::VIEW_ORIENTATION_AXIAL;
    case medTableWidgetItem::CoronalType:
        return medImageView::VIEW_ORIENTATION_CORONAL;
    case medTableWidgetItem::SagittalType:
        return medImageView::VIEW_ORIENTATION_SAGITTAL;
    case medTableWidgetItem::ThreeDimType:
        return medImageView::VIEW_ORIENTATION_3D;
    default:
        return medImageView::VIEW_ALL_ORIENTATION;
    }
}
