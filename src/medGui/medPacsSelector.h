/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

class medToolBoxPacsSelectorPrivate;

#include "medGuiExport.h"
#include <QWidget>

class MEDGUI_EXPORT medPacsSelector : public QWidget
{
    Q_OBJECT

public:
    medPacsSelector(QWidget *parent = 0);
    ~medPacsSelector();

signals:
    void selectionChanged(QVector<int> list);

 public slots:

    void updateList();

    void emitSelectionChanged();

private:

    void readSettings();

    void fillWidget( QList<QVariant> nodes );

    medToolBoxPacsSelectorPrivate *d;
};

