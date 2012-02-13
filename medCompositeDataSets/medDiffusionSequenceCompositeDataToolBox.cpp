#include <medDiffusionSequenceCompositeDataToolBox.h>

#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>

#include <medCore/medAbstractView.h>
#include <medCore/medRunnableProcess.h>
#include <medCore/medJobManager.h>
#include <medCore/medDataReaderWriter.h>
#include <medDatabaseController.h>

#include <medDropSite.h>
#include <medToolBoxFactory.h>
#include <medToolBoxCompositeDataSetImporter.h>
#include <medCompositeDataSetsWriter.h>
#include <medProgressionStack.h>

#include <VectorWidget.h>

medDiffusionSequenceCompositeDataToolBox::medDiffusionSequenceCompositeDataToolBox(QWidget* parent): medToolBoxCompositeDataSetImporterCustom(parent) {
    QWidget*     widget = new QWidget(this);
    QFormLayout* layout = new QFormLayout(widget);

    reset();

    setAcceptDrops(true);
    widget->setAcceptDrops(true);

    setTitle(tr("Images and gradients"));
    layout->addRow(&table);

    QObject::connect(&table,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(onItemClicked(QTableWidgetItem*)));
    QObject::connect(&table,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onContextTreeMenu(QPoint)));
    QObject::connect(&table,SIGNAL(newItem(const QString&,bool&)),this,SLOT(onNewItem(const QString&,bool&)));

    addWidget(widget);
}

medDiffusionSequenceCompositeDataToolBox::~medDiffusionSequenceCompositeDataToolBox() {
#if 0
    delete d;
    d = NULL;
#endif
}

bool medDiffusionSequenceCompositeDataToolBox::registered() {
    return medToolBoxFactory::instance()->registerCustomCompositeDataSetImporterToolBox(
                "medDiffusionSequenceCompositeDataToolBox",
                tr("Diffusion Sequence Importer"),
                tr("Diffusion Sequence Importer"),
                medDiffusionSequenceCompositeDataToolBox::create);
}

QString medDiffusionSequenceCompositeDataToolBox::description() const {
    return QString(tr("Diffusion Sequence Importer"));
}

medToolBoxCompositeDataSetImporterCustom* medDiffusionSequenceCompositeDataToolBox::create(QWidget* parent) {
    return new medDiffusionSequenceCompositeDataToolBox(parent);
}

void medDiffusionSequenceCompositeDataToolBox::onNewItem(const QString& item,bool& valid) { 
    dtkAbstractData* volume = medDiffusionSequenceCompositeData::readVolume(item);
    if (volume!=0)
        valid = true;
}

void medDiffusionSequenceCompositeDataToolBox::onItemClicked(QTableWidgetItem* item) { 
    if (item->column()==1)
        table.editItem(item);
}

void medDiffusionSequenceCompositeDataToolBox::onContextTreeMenu(QPoint) {  
}

void medDiffusionSequenceCompositeDataToolBox::reset()  { table.clear(); }
void medDiffusionSequenceCompositeDataToolBox::load()   { import(false); }
bool medDiffusionSequenceCompositeDataToolBox::import() { return import(true);  }

//  Import the data through the writer.

bool medDiffusionSequenceCompositeDataToolBox::import(const bool persistent) {

    QStringList                                         vol_list;
    medDiffusionSequenceCompositeData::GradientListType grad_list;

    for (int i=0;i<table.rowCount();++i) {
        const QTableWidgetItem* item0 = table.item(i,0);
        const QTableWidgetItem* item1 = table.item(i,1);
        vol_list << item0->data(Qt::UserRole).toString();
        const QStringList values = item1->data(Qt::EditRole).toString().split(",");
        medDiffusionSequenceCompositeData::GradientType grad;
        for (int i=0;i<3;++i)
            grad[i] = values.at(i).simplified().toDouble();
        grad_list.push_back(grad);
    }

    medDiffusionSequenceCompositeData* cds = new medDiffusionSequenceCompositeData();
    cds->readVolumes(vol_list,true);
    cds->setGradientList(grad_list);

    const QString& uuid = QUuid::createUuid().toString().replace("{","").replace("}","")+".cds";
    medInria::DataReaderWriter::write(uuid,cds);

    medDatabaseController::instance()->import(uuid,!persistent);
    reset();

    return true;
}
