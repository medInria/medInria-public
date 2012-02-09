#include "medPluginWidget.h"

#include <medPluginManager.h>

#include <dtkCore/dtkPlugin.h>
#include <dtkGui/dtkAboutPlugin.h>

#include <QtGui>

class medPluginWidgetPrivate
{
public:
    QTreeWidget* pluginsTree;
    QTableWidget* errorTable;

    void resetPluginsTree();
    void resetFailedPluginsTable();
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
}

void medPluginWidgetPrivate::resetFailedPluginsTable()
{
    //First clear the content
    errorTable->clearContents();

    //get the list of plugins
    medPluginManager* mpm = medPluginManager::instance();
    errorTable->setRowCount(mpm->loadErrors().size());

    for(unsigned int i=0; i < mpm->loadErrors().size(); i++)
    {
        qDebug()<< "display error" << mpm->loadErrors()[i];
        QTableWidgetItem * item = new QTableWidgetItem(mpm->loadErrors()[i]);
        errorTable->setItem(i,0,item);
    }
    errorTable->resizeColumnToContents(0);
}


medPluginWidget::medPluginWidget(QWidget *parent) :
    QWidget(parent), d(new medPluginWidgetPrivate)
{
    QVBoxLayout * layout = new QVBoxLayout(this);
    QLabel * pluginListLabel = new QLabel(tr("List of Plugins"),this);
    QLabel * failedPluginsLabel = new QLabel(tr("Failed Plugins"),this);

    //Initialise the tree Widget
    d->pluginsTree = new QTreeWidget(this);
    d->pluginsTree->setFrameStyle(QFrame::NoFrame);
    d->pluginsTree->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->pluginsTree->setUniformRowHeights(true);
    d->pluginsTree->setAlternatingRowColors(true);
    d->pluginsTree->setAnimated(true);
    d->pluginsTree->setSortingEnabled(true);
    d->pluginsTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->pluginsTree->setSelectionMode(QAbstractItemView::SingleSelection);
    d->pluginsTree->header()->setStretchLastSection(true);
    d->pluginsTree->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    d->pluginsTree->setColumnCount(3);
    QStringList treeColumns;
    treeColumns << tr("Name") << tr("Version") << tr("Authors");
    d->pluginsTree->setHeaderLabels(treeColumns);

    connect(d->pluginsTree,SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(onPluginTreeItemActivated(QTreeWidgetItem*,int)));

    //Initialise the error table
    d->errorTable = new QTableWidget(this);
    d->errorTable->setFrameStyle(QFrame::NoFrame);
    d->errorTable->setAttribute(Qt::WA_MacShowFocusRect, false);
    d->errorTable->setAlternatingRowColors(true);
    d->errorTable->setSortingEnabled(true);
    d->errorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->errorTable->setSelectionMode(QAbstractItemView::SingleSelection);
    d->errorTable->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    d->errorTable->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    d->errorTable->setColumnCount(1);
    QStringList tableColumns;
    tableColumns << tr("Error Messages");
    d->errorTable->setHorizontalHeaderLabels(tableColumns);

    connect(d->errorTable,SIGNAL(itemActivated(QTableWidgetItem*)),
            this,SLOT(onErrorTableItemActivated(QTableWidgetItem*)));


    layout->addWidget(pluginListLabel);
    layout->addWidget(d->pluginsTree);
    layout->addWidget(failedPluginsLabel);
    layout->addWidget(d->errorTable);
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
    d->resetFailedPluginsTable();
}

void medPluginWidget::onPluginTreeItemActivated(QTreeWidgetItem *item, int column)
{
    Q_UNUSED (column);
    QDialog * dial = new QDialog(this);
    dtkPlugin * plugin = medPluginManager::instance()->plugin(item->text(0));
    QString windowTitle = tr("medInria: about ");
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

void medPluginWidget::onErrorTableItemActivated(QTableWidgetItem* item)
{
    QDialog * dial = new QDialog(this);

    QString windowTitle = tr("medInria: error message");
    dial->setWindowTitle(windowTitle);

    QVBoxLayout * layout = new QVBoxLayout(dial);

    QTextEdit * message = new QTextEdit(dial);
    message->setReadOnly(true);
    message->setText(item->text());

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


