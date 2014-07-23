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

#include <QTableWidget>

class medLayoutChooserPrivate;

class MEDCORE_EXPORT medLayoutChooser : public QTableWidget
{
    Q_OBJECT

public:
     medLayoutChooser(QWidget *parent = 0);
    ~medLayoutChooser();

    QSize sizeHint() const;

    int sizeHintForRow(int row) const;
    int sizeHintForColumn(int column) const;

signals:
    void selected(int rows, int cols);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

protected slots:
    void onSelectionChanged(const QItemSelection& selected, const QItemSelection& unselected);

private:
    medLayoutChooserPrivate *d;
};


