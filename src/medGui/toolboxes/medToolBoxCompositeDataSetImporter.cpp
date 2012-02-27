/* medToolBoxCompositeDataSetImporter ---
 *
 * Author: Nicolas Toussaint

 * Change log:
 *
 */

#include <medCore/medMessageController.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medToolBoxCompositeDataSetImporter.h>
#include <medToolBoxCompositeDataSetImporterCustom.h>

class medToolBoxCompositeDataSetImporterPrivate
{
public:

  QHash<QString, medToolBoxCompositeDataSetImporterCustom*> toolBoxes;

  medToolBoxCompositeDataSetImporterCustom* currentToolBox;

  // methods
  void read(QString filename);

  // member
  QWidget* parent;
  QVBoxLayout* customContainerLayout;

  QComboBox* type;
  QPushButton* import;
  QPushButton* reset;
  QPushButton* cancel;
  bool isInitialized;
};

medToolBoxCompositeDataSetImporter::medToolBoxCompositeDataSetImporter(QWidget *parent): medToolBox(parent), d(new medToolBoxCompositeDataSetImporterPrivate) {
    d->isInitialized = false;
    d->currentToolBox = 0;
    this->setTitle(tr("Composite DataSet Import"));
    this->initialize();
}

medToolBoxCompositeDataSetImporter::~medToolBoxCompositeDataSetImporter(void)
{
    delete d;
    d = NULL;
}

void medToolBoxCompositeDataSetImporterPrivate::read(QString filename) { }

void medToolBoxCompositeDataSetImporter::onImportClicked() {
    if (!d->currentToolBox) {
        this->showError(this,tr("Select a type first"),3000);
        return;
    }
    d->currentToolBox->import();
}

void medToolBoxCompositeDataSetImporter::onCancelClicked() {
    if (!d->currentToolBox)
        return;
    d->currentToolBox->cancel();
}

void medToolBoxCompositeDataSetImporter::onResetClicked() {
    if (!d->currentToolBox)
        return;
    d->currentToolBox->reset();
}

void medToolBoxCompositeDataSetImporter::initialize()
{
    if (d->isInitialized)
        return;
    QWidget* mainwidget = new QWidget(this);

    int buttonWidth = 100;

    QVBoxLayout * vLayout = new QVBoxLayout();
    mainwidget->setLayout (vLayout);

    // the row of buttons
    QHBoxLayout * topLayout = new QHBoxLayout();
    topLayout->addStretch(0);

    d->type = new QComboBox(mainwidget);
    d->type->addItem(tr("Select data type"));
    d->type->setToolTip(tr("Choose a type of composite data set to import"));

    int i=1;
    medToolBoxFactory* factory = medToolBoxFactory::instance();
    foreach (QString toolbox,factory->compositeDataSetImporterToolBoxes())
    {
//        d->type->addItem(factory->compositeToolBoxDetailsFromId(toolbox).second,toolbox);
        QPair<QString, QString> pair = factory->compositeToolBoxDetailsFromId(
                    toolbox);
        QString name = pair.first;
        QString descr = pair.second;
        d->type->addItem(name, toolbox);
        d->type->setItemData(i, descr, Qt::ToolTipRole);
        i++;
    }

    connect(d->type,SIGNAL(activated(int)),this,SLOT(onCurrentTypeChanged(int)));

    topLayout->addWidget(d->type);
    topLayout->addStretch(1);
    vLayout->addLayout(topLayout);

    d->customContainerLayout = new QVBoxLayout();

    vLayout->addLayout (d->customContainerLayout);

    //import button
    d->import = new QPushButton (tr("Import"),mainwidget);
    d->import->setMaximumWidth(buttonWidth);
    connect(d->import,SIGNAL(clicked()),
        this,SLOT(onImportClicked()));

    //cancel button
    d->cancel = new QPushButton (tr("Cancel"),mainwidget);
    d->cancel->setMaximumWidth(buttonWidth);
    connect(d->cancel,SIGNAL(clicked()),
        this,SLOT(onCancelClicked()));

    //reset button
    d->reset = new QPushButton (tr("Reset"), mainwidget);
    d->reset->setMaximumWidth(buttonWidth);
    connect(d->reset,SIGNAL(clicked()),this,SLOT(onResetClicked()));

    // the row of buttons
    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(d->reset,1);
    buttonLayout->addWidget(d->cancel,1);
    buttonLayout->addWidget(d->import,1);
    vLayout->addLayout(buttonLayout);

    d->reset->hide ();
    d->cancel->hide ();
    d->import->hide ();

    this->addWidget (mainwidget);

    // connections
    connect(this,SIGNAL(showError(QObject*,const        QString&,unsigned int)),
        medMessageController::instance(),SLOT(showError (QObject*,const QString&,unsigned int)));
    connect(this,SIGNAL(showInfo(QObject*,const        QString&,unsigned int)),
        medMessageController::instance(),SLOT(showInfo (QObject*,const QString&,unsigned int)));


    d->isInitialized = true;
}


bool medToolBoxCompositeDataSetImporter::import()
{
    return true;
}


void medToolBoxCompositeDataSetImporter::onCurrentTypeChanged(const int i) {

    medToolBoxCompositeDataSetImporterCustom* toolbox = NULL;
    const QString id = d->type->itemData(i).toString();

    if (d->toolBoxes.contains(id))
        toolbox = d->toolBoxes[id];
    else {
        toolbox = medToolBoxFactory::instance()->createCustomCompositeDataSetImporterToolBox(id, this);
        if (toolbox) {
            toolbox->setStyleSheet("medToolBoxBody {border:none;padding:0px}");
            toolbox->header()->hide();

            connect (toolbox, SIGNAL (success()), this, SIGNAL (success()));
            connect (toolbox, SIGNAL (failure()), this, SIGNAL (failure()));

            d->toolBoxes[id] = toolbox;
        }
    }

    if (!toolbox) {
        if (d->currentToolBox) {
            d->currentToolBox->hide();
            d->customContainerLayout->removeWidget ( d->currentToolBox );
            d->currentToolBox = 0;
        }
        d->reset->hide ();
        d->cancel->hide ();
        d->import->hide ();
        return;
    }

    toolbox->setCompositeDataSetImporterToolBox(this);

    //get rid of old toolBox
    if (d->currentToolBox) {
        d->currentToolBox->hide();
        d->customContainerLayout->removeWidget ( d->currentToolBox );
        d->currentToolBox = 0;
    }

    toolbox->show();
    d->customContainerLayout->addWidget ( toolbox );

    d->currentToolBox = toolbox;

    d->reset->show ();
    d->cancel->show ();
    d->import->show ();
}
