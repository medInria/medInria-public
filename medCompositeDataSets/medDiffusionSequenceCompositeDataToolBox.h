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

    static bool registered(void);

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


