/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medPluginWidget.h>

#include <medPluginManager.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkPlugin.h>
#include <dtkGui/dtkAboutPlugin.h>

#include <QtGui>


////////////////////////////////////////////////////////////////////////////////
//medPluginWidgetPrivate
////////////////////////////////////////////////////////////////////////////////

class medPluginWidgetPrivate
{
public:
    QTreeWidget* pluginsTree;
    QTreeWidget* typesTree;
    QTreeWidget* errorTree;

    void resetPluginsTree();
    void resetTypesTree();
    void resetFailedPluginsTree();
};


void medPluginWidgetPrivate::resetPluginsTree()
{
    //First clear the content
    pluginsTree->clear();

    //get the list of plugins
    medPluginManager* mpm = medPluginManager::instance();

    foreach(dtkPlugin* plugin,mpm->plugins())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(pluginsTree);
        item->setText(0,plugin->name());
        item->setText(1,plugin->version());

        int authorsSize = plugin->authors().size();
        QString author;
        if ( authorsSize )
            author = plugin->authors()[0];
        if ( authorsSize > 1)
            author += " et al.";
        item->setText(2,author);

        pluginsTree->addTopLevelItem(item);

    }
    pluginsTree->resizeColumnToContents(0);
    pluginsTree->setColumnWidth(1,90);
    pluginsTree->sortByColumn(0,Qt::AscendingOrder);
}

void medPluginWidgetPrivate::resetTypesTree()
{
    //First clear the content
    typesTree->clear();

    //get the list of plugins

    medAbstractDataFactory * dataFactory = medAbstractDataFactory::instance();
    QTreeWidgetItem * dataItem = new QTreeWidgetItem(typesTree);
    dataItem->setText(0,QObject::tr("Data"));
    foreach(QString data,dataFactory->creators())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(dataItem);
        item->setText(1,data);

    }
    typesTree->addTopLevelItem(dataItem);

    QTreeWidgetItem * readerItem = new QTreeWidgetItem(typesTree);
    readerItem->setText(0,QObject::tr("Readers"));
    foreach(QString data,dataFactory->readers())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(readerItem);
        item->setText(1,data);

    }
    typesTree->addTopLevelItem(readerItem);

    QTreeWidgetItem * writerItem = new QTreeWidgetItem(typesTree);
    writerItem->setText(0,QObject::tr("Writers"));
    foreach(QString data,dataFactory->writers())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(writerItem);
        item->setText(1,data);

    }
    typesTree->addTopLevelItem(writerItem);


    dtkAbstractViewFactory * viewFactory =
            dtkAbstractViewFactory::instance();

    QTreeWidgetItem * viewItem = new QTreeWidgetItem(typesTree);
    viewItem->setText(0,QObject::tr("Views"));
    foreach(QString view,viewFactory->creators())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(viewItem);
        item->setText(1,view);

    }
    typesTree->addTopLevelItem(viewItem);

    dtkAbstractProcessFactory * processFactory =
            dtkAbstractProcessFactory::instance();

    QTreeWidgetItem * processItem = new QTreeWidgetItem(typesTree);
    processItem->setText(0,QObject::tr("Processes"));
    foreach(QString process,processFactory->creators())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(processItem);
        item->setText(1,process);

    }
    typesTree->addTopLevelItem(processItem);
    typesTree->setColumnWidth(1,150);
    typesTree->sortByColumn(1,Qt::AscendingOrder);
    typesTree->sortByColumn(0,Qt::AscendingOrder);

}

void medPluginWidgetPrivate::resetFailedPluginsTree()
{
    //First clear the content
    errorTree->clear();

    //get the list of plugins
    medPluginManager* mpm = medPluginManager::instance();

    foreach(QString error, mpm->loadErrors())
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(errorTree);
        item->setText(0,error);
        errorTree->addTopLevelItem(item);
    }
    errorTree->resizeColumnToContents(0);
}

////////////////////////////////////////////////////////////////////////////////
//medPluginWidget methods
////////////////////////////////////////////////////////////////////////////////
medPluginWidget::medPluginWidget(QWidget *parent) :
    QTabWidget(parent), d(new medPluginWidgetPrivate)
{
    //Initialise the tree Widget
    d->pluginsTree = new QTreeWidget(this);
    d->pluginsTree->setFrameStyle(QFrame::NoFrame);
    //d->pluginsTree->setMinimumWidth(700);

    d->pluginsTree->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->pluginsTree->setUniformRowHeights(true);
    d->pluginsTree->setAlternatingRowColors(true);
    d->pluginsTree->setAnimated(true);
    d->pluginsTree->setSortingEnabled(true);
    d->pluginsTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->pluginsTree->setSelectionMode(QAbstractItemView::SingleSelection);
    d->pluginsTree->header()->setStretchLastSection(true);
    d->pluginsTree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->pluginsTree->setColumnCount(3);
    QStringList treeColumns;
    treeColumns << tr("Name") << tr("Version") << tr("Authors");
    d->pluginsTree->setHeaderLabels(treeColumns);
    d->pluginsTree->setColumnWidth(1, 50);

    connect(d->pluginsTree,SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(onPluginTreeItemActivated(QTreeWidgetItem*,int)));

    //Initialise the error Tree
    d->errorTree = new QTreeWidget(this);
    d->errorTree->setFrameStyle(QFrame::NoFrame);
    //d->errorTree->setMinimumWidth(700);
    d->errorTree->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->errorTree->setAlternatingRowColors(true);
    d->errorTree->setSortingEnabled(true);
    d->errorTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->errorTree->setSelectionMode(QAbstractItemView::SingleSelection);
    d->errorTree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->errorTree->setColumnCount(1);
    QStringList errorColumns;
    errorColumns << tr("Error Messages");
    d->errorTree->setHeaderLabels(errorColumns);

    connect(d->errorTree,SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this,SLOT(onErrorTreeItemActivated(QTreeWidgetItem*,int)));

    //Initialise list of types
    d->typesTree = new QTreeWidget(this);


    d->typesTree->setFrameStyle(QFrame::NoFrame);
    //d->typesTree->setMinimumWidth(700);

    d->typesTree->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->typesTree->setUniformRowHeights(true);
    d->typesTree->setAlternatingRowColors(true);
    d->typesTree->setAnimated(true);
    d->typesTree->setSortingEnabled(true);
    d->typesTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->typesTree->setSelectionMode(QAbstractItemView::SingleSelection);
    //d->typesTree->header()->setStretchLastSection(true);
    d->typesTree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->typesTree->setColumnCount(2);
    QStringList typesTreeColumns;
    typesTreeColumns << tr("Category") << tr("Name");// << tr("Plugin");
    d->typesTree->setHeaderLabels(typesTreeColumns);

    //connect(d->typesTree,SIGNAL(itemActivated(QTreeWidgetItem*,int)),
    //        this, SLOT(onTypesTreeItemActivated(QTreeWidgetItem*,int)));

    this->addTab(d->pluginsTree,tr("Loaded Plugins"));
    this->addTab(d->typesTree,tr("Loaded types"));
    this->addTab(d->errorTree,tr("Failed Plugins"));
    reset();
}

medPluginWidget::~medPluginWidget()
{
    delete d;
    d = NULL;
}

void medPluginWidget::reset()
{
    d->resetPluginsTree();
    d->resetFailedPluginsTree();
    d->resetTypesTree();
}

void medPluginWidget::onPluginTreeItemActivated(QTreeWidgetItem *item, int column)
{
    Q_UNUSED (column);
    QDialog * dial = new QDialog(this);
    dtkPlugin * plugin = medPluginManager::instance()->plugin(item->text(0));
    QString windowTitle = tr("MUSIC: about ");
    windowTitle += plugin->name();
    dial->setWindowTitle(windowTitle);
    dtkAboutPlugin * apWidget = new dtkAboutPlugin(plugin,dial);

    QVBoxLayout * layout = new QVBoxLayout(dial);

    QPushButton * okBut = new QPushButton(dial);
    okBut->setText("Ok");
    okBut->setFocusPolicy(Qt::NoFocus);
    QObject::connect(okBut, SIGNAL(clicked()), dial, SLOT(close()));

    QHBoxLayout * butLayout = new QHBoxLayout;
    butLayout->addStretch();
    butLayout->addWidget(okBut);
    butLayout->addStretch();

    layout->addWidget(apWidget);
    layout->addLayout(butLayout);

    dial->setLayout(layout);
    dial->exec();
}

void medPluginWidget::onErrorTreeItemActivated(QTreeWidgetItem* item,int column)
{
    Q_UNUSED (column);
    QDialog * dial = new QDialog(this);

    QString windowTitle = tr("MUSIC: error message");
    dial->setWindowTitle(windowTitle);

    QVBoxLayout * layout = new QVBoxLayout(dial);

    QTextEdit * message = new QTextEdit(dial);
    message->setReadOnly(true);
    message->setText(item->text(0));

    QPushButton * okBut = new QPushButton(dial);
    okBut->setText("Ok");
    okBut->setFocusPolicy(Qt::NoFocus);
    QObject::connect(okBut, SIGNAL(clicked()), dial, SLOT(close()));

    QHBoxLayout * butLayout = new QHBoxLayout;
    butLayout->addStretch();
    butLayout->addWidget(okBut);
    butLayout->addStretch();

    layout->addWidget(message);
    layout->addLayout(butLayout);

    dial->setLayout(layout);
    dial->exec();
}


