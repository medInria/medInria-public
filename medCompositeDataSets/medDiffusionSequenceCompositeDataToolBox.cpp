#include <medDiffusionSequenceCompositeDataToolBox.h>

#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>

#include <medCore/medAbstractView.h>
#include <medCore/medRunnableProcess.h>
#include <medCore/medJobManager.h>

#include <medGui/medToolBoxFactory.h>
#include <medGui/medToolBoxCompositeDataSetImporter.h>
#include <medGui/medProgressionStack.h>


class medDiffusionSequenceCompositeDataToolBoxPrivate {
public:
    QTreeWidget * tree;
    medDiffusionSequenceCompositeDataToolBox::GradientListType gradients;
    QVector<dtkAbstractData*> volumeList;
    QList<QTreeWidgetItem*> itemList;
};

medDiffusionSequenceCompositeDataToolBox::medDiffusionSequenceCompositeDataToolBox(QWidget *parent): medToolBoxCompositeDataSetImporterCustom(parent),d(new medDiffusionSequenceCompositeDataToolBoxPrivate)
{
    d->tree = new QTreeWidget(this);

    d->tree->setColumnCount(2);
    d->tree->setColumnWidth(10,80);
    d->tree->setSelectionMode(QAbstractItemView::NoSelection);
    QStringList headers;
    headers << "Image" << "Gradient";
    d->tree->setHeaderLabels(headers);
    d->tree->setAnimated(true);
    d->tree->setAlternatingRowColors(true);
    d->tree->setRootIsDecorated(false);
    d->tree->setContextMenuPolicy(Qt::CustomContextMenu);

    this->setTitle("images and gradients");
    this->addWidget(d->tree);

    QObject::connect(d->tree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onItemClicked(QTreeWidgetItem *, int)));
    QObject::connect(d->tree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextTreeMenu(QPoint)));

    this->addVolumeToTree(0, QString("dtiimage[0]"));
    GradientType tmpgradient;
    tmpgradient[0] = 1;
    tmpgradient[1] = -0.5;
    tmpgradient[2] = -0.3;
    
    this->addGradientToTree(0, tmpgradient);
}

medDiffusionSequenceCompositeDataToolBox::~medDiffusionSequenceCompositeDataToolBox(void)
{
    delete d;
    d = NULL;
}

bool medDiffusionSequenceCompositeDataToolBox::registered(void)
{
    qDebug() << "diffusion gradient toolbox registered ";
    return medToolBoxFactory::instance()->
            registerCustomCompositeDataSetImporterToolBox("medDiffusionSequenceCompositeDataToolBox",
                                 createMedDiffusionCompositeDataToolBox);
}

QString medDiffusionSequenceCompositeDataToolBox::description (void) const
{
    return QString("Composite Data");
}


medToolBoxCompositeDataSetImporterCustom *createMedDiffusionCompositeDataToolBox(QWidget *parent)
{
    return new medDiffusionSequenceCompositeDataToolBox (parent);
}

void medDiffusionSequenceCompositeDataToolBox::onItemClicked (QTreeWidgetItem *, int)
{ 
}

void medDiffusionSequenceCompositeDataToolBox::onContextTreeMenu (QPoint)
{  
}

void medDiffusionSequenceCompositeDataToolBox::reset (void)
{
    // empty the file list of data
    d->tree->clear();
    d->itemList.clear();
}

void medDiffusionSequenceCompositeDataToolBox::cancel (void)
{
    this->reset();
}

bool medDiffusionSequenceCompositeDataToolBox::import (void)
{
    // actually import the data through the writer and the header thing, zip ;-)
    qDebug() << "importing data...";

    return this->writeInDataBase();
}

void medDiffusionSequenceCompositeDataToolBox::addVolumeToTree (unsigned int index, QString volumename)
{
    QString itemstring = volumename;
    QTreeWidgetItem * item = new QTreeWidgetItem(d->tree->invisibleRootItem(), QTreeWidgetItem::UserType+1);
    item->setText(0, itemstring);
    item->setIcon(0,QIcon(":icons/layer.png"));
    d->itemList.push_back (item);
}

void medDiffusionSequenceCompositeDataToolBox::readVolumes (QStringList paths)
{
    QList<QString> readers = dtkAbstractDataFactory::instance()->readers();
  
    for (int i=0; i<paths.size(); i++) {
        QString filepath = paths[i];
        dtkAbstractDataReader* reader = NULL;
        
        for (int i=0; i<readers.size(); i++) {
            dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i]);
            if (dataReader->canRead( filepath ))
                reader = dataReader;
            else
                delete reader;
        }

        reader->readInformation( filepath );
        dtkAbstractData* volume = reader->data();
        QString description = volume->description();
        if (!description.contains ("Image")) {
            // emit medToolBoxCompositeDataSetImporter::showError (this, tr ("file does not describe any known image type"), 3000);
            continue;
        }
        if (description.contains ("Image4D")) {
            // emit medToolBoxCompositeDataSetImporter::showError (this, tr ("4D image is not supported yet"), 3000);
            continue;
        }
        if (!description.contains ("Image3D")) {
            // emit medToolBoxCompositeDataSetImporter::showError (this, tr ("image should be 3D"), 3000);
            continue;
        }
        
        d->volumeList.push_back (volume);
        this->addVolumeToTree (0, filepath);
    }  
}

void medDiffusionSequenceCompositeDataToolBox::addGradientToTree (unsigned int imageindex, GradientType gradient) {

    QTreeWidgetItem * item = new QTreeWidgetItem(d->tree->invisibleRootItem(), QTreeWidgetItem::UserType+1);
    std::ostringstream os;
    os << "["<<gradient[0]<<", "<<gradient[1]<<","<<gradient[2]<<"]";
    qDebug() << os.str().c_str();

    item->setText(1, os.str().c_str());
    //d->tree->setItemWidget(item, 2, NULL);
}

void medDiffusionSequenceCompositeDataToolBox::readGradients (QString filepath) {
    GradientReaderType::Pointer reader = GradientReaderType::New();

    reader->SetFileName (filepath.toAscii().data());
    reader->Update();
    d->gradients = reader->GetGradientList();
    for (unsigned int i=0; i<d->gradients.size(); i++)
        this->addGradientToTree (i, d->gradients[i]);
}

bool medDiffusionSequenceCompositeDataToolBox::writeInDataBase(void) {
    // instantiate the data
    medDiffusionSequenceCompositeData* diffusionsequence = new medDiffusionSequenceCompositeData();
    diffusionsequence->setGradientList (d->gradients);
    diffusionsequence->setVolumeList (d->volumeList);

    // instantiate the writer
    medDiffusionSequenceCompositeDataWriter* writer = new medDiffusionSequenceCompositeDataWriter();
    writer->setData (diffusionsequence);
    writer->write();

    return 1;
}
