/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medLayoutChooser.h"

#include <QtGui>

class medLayoutChooserPrivate
{
public:
    int left;
    int right;
    int top;
    int bottom;
};

medLayoutChooser::medLayoutChooser(QWidget *parent) : QTableWidget(parent), d(new medLayoutChooserPrivate)
{
    this->setRowCount(5);
    this->setColumnCount(5);
    this->horizontalHeader()->setHidden(true);
    this->verticalHeader()->setHidden(true);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->resizeColumnsToContents();
    this->resizeRowsToContents();
    this->setFrameShape(QFrame::NoFrame);

    d->left = d->right = d->top = d->bottom = 0;

    connect(this->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));

    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

medLayoutChooser::~medLayoutChooser(void)
{
    delete d;

    d = NULL;
}

QSize medLayoutChooser::sizeHint(void) const
{
    return QSize(this->columnCount()*30, this->rowCount()*30);
}

int medLayoutChooser::sizeHintForRow(int row) const
{
    Q_UNUSED(row);

    return 30;
}

int medLayoutChooser::sizeHintForColumn(int column) const
{
    Q_UNUSED(column);

    return 30;
}

void medLayoutChooser::mousePressEvent(QMouseEvent *event)
{
    d->left = d->right = d->top = d->bottom = 0;

    QTableWidget::mousePressEvent(event);
}

void medLayoutChooser::mouseReleaseEvent(QMouseEvent *event)
{
    emit selected(d->bottom-d->top+1, d->right-d->left+1);

    QTableWidget::mouseReleaseEvent(event);

    this->clearSelection();

    d->left   = 0;
    d->right  = 0;
    d->top    = 0;
    d->bottom = 0;
}

void medLayoutChooser::onSelectionChanged(const QItemSelection& selected, const QItemSelection& unselected)
{
    Q_UNUSED(unselected);

    if(!selected.count())
        return;

    d->left   = qMin(d->left,   selected.first().left());
    d->right  = qMax(d->right,  selected.first().right());
    d->top    = qMin(d->top,    selected.first().top());
    d->bottom = qMax(d->bottom, selected.first().bottom());
}
