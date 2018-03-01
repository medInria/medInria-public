/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCompositeDataSetImporterAbstractToolBox.h>

#include <medDiffusionSequenceCompositeData.h>
#include <itkGradientFileReader.h>
#include <DiffusionSequenceWidget.h>

class medDiffusionSequenceCompositeDataToolBox: public medCompositeDataSetImporterAbstractToolBox {
    Q_OBJECT

public:

     medDiffusionSequenceCompositeDataToolBox(QWidget* parent=0);
    ~medDiffusionSequenceCompositeDataToolBox();

    static bool registered();

    QString description() const;

public slots:

    bool import();
    void reset();
    void load();

    void onItemClicked(QTableWidgetItem*);
    void onContextTreeMenu(QPoint);
    void onNewItem(const QString&,bool&);

private:

    DiffusionSequenceWidget table;

    bool import(const bool persistent);
};


