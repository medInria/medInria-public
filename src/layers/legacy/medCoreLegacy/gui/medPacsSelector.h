#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

class medToolBoxPacsSelectorPrivate;

#include <QWidget>

#include <medCoreLegacyExport.h>

class MEDCORELEGACY_EXPORT medPacsSelector : public QWidget
{
    Q_OBJECT

public:
    medPacsSelector(QWidget *parent = nullptr);
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
