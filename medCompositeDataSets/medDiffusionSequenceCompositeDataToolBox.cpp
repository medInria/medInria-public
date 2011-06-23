#include <medDiffusionSequenceCompositeDataToolBox.h>

#include <QtGui>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewFactory.h>

#include <medCore/medAbstractView.h>
#include <medCore/medRunnableProcess.h>
#include <medCore/medJobManager.h>

#include <medGui/medToolBoxFactory.h>
#include <medGui/medToolBoxCompositeDataSetImporter.h>
#include <medGui/medProgressionStack.h>

class medDiffusionSequenceCompositeDataToolBoxPrivate
{
public:
    QTreeWidget * tree;
};

medDiffusionSequenceCompositeDataToolBox::medDiffusionSequenceCompositeDataToolBox(QWidget *parent) : medToolBoxCompositeDataSetImporterCustom(parent), d(new medDiffusionSequenceCompositeDataToolBoxPrivate)
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

    this->addFileToTree(QString("dtiimage[0]"));

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

bool medDiffusionSequenceCompositeDataToolBox::import (void)
{
  // actually import the data through the writer and the header thing, zip ;-)
  
}


void medDiffusionSequenceCompositeDataToolBox::reset (void)
{
  // empty the file list of data
}


void medDiffusionSequenceCompositeDataToolBox::cancel (void)
{
  this->reset();
}

void medDiffusionSequenceCompositeDataToolBox::addFilesToTree (QStringList paths)
{  
}

void medDiffusionSequenceCompositeDataToolBox::addFileToTree (QString path)
{
    QString layerItemString = path;
    QTreeWidgetItem * layerItem = new QTreeWidgetItem(d->tree->invisibleRootItem(), QTreeWidgetItem::UserType+1);
    layerItem->setText(0, layerItemString);
    layerItem->setIcon(0,QIcon(":icons/layer.png"));
}

void medDiffusionSequenceCompositeDataToolBox::onItemClicked (QTreeWidgetItem *, int)
{
  
}

void medDiffusionSequenceCompositeDataToolBox::onContextTreeMenu (QPoint)
{
  
}
