/* medToolBoxCompositeDataSetImporter ---
 * 
 * Author: Nicolas Toussaint

 * Change log:
 * 
 */

#include "medToolBoxCompositeDataSetImporter.h"

#include <medCore/medMessageController.h>

#include <QComboBox>



class medToolBoxCompositeDataSetImporterPrivate
{
public:
  // methods
  void read(QString filename);
  
  // member
  QWidget* parent;
  QComboBox* type;
  QPushButton* add;
  QListView* filelist;
  QPushButton* import;
  QPushButton* reset;
  QPushButton* cancel;
  bool isInitialized;
};

medToolBoxCompositeDataSetImporter::medToolBoxCompositeDataSetImporter(QWidget *parent) : medToolBox(parent), d(new medToolBoxCompositeDataSetImporterPrivate)
{
    d->isInitialized = false;
    this->setTitle("Composite DataSet Import");
    this->initialize();
}

medToolBoxCompositeDataSetImporter::~medToolBoxCompositeDataSetImporter(void)
{
    delete d;
    d = NULL;
}

void medToolBoxCompositeDataSetImporterPrivate::read(QString filename)
{
}

void medToolBoxCompositeDataSetImporter::onImportClicked()
{
}

void medToolBoxCompositeDataSetImporter::onCancelClicked()
{
}

void medToolBoxCompositeDataSetImporter::onResetClicked()
{
    d->read(QString ("myimage.mha"));
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
    d->type->addItem (tr ("select data type"));
    d->type->addItem (tr ("DWI"));
    d->type->insertSeparator (2);
    connect(d->type,SIGNAL(currentIndexChanged(int index)),
	    this,SLOT(onCurrentTypeChanged(int index)));
    
    d->add = new QPushButton (tr ("Add Files..."), mainwidget);
    d->add->setMaximumWidth(buttonWidth);
    connect(d->add,SIGNAL(clicked()),
        this,SLOT(onAddClicked()));
    
    topLayout->addWidget(d->type);
    topLayout->addStretch(1);
    topLayout->addWidget(d->add);
    vLayout->addLayout(topLayout);

    d->filelist = new QListView(mainwidget);
    d->filelist->setFlow (QListView::TopToBottom);
    vLayout->addWidget(d->filelist);
    
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
    connect(d->reset,SIGNAL(clicked()),
        this,SLOT(onResetClicked()));

    // the row of buttons
    QHBoxLayout * buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(d->reset,1);
    buttonLayout->addWidget(d->cancel,1);
    buttonLayout->addWidget(d->import,1);
    vLayout->addLayout(buttonLayout);
    
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
    bool success = true;

    if (success)
    {
      emit (showInfo(this,
		     tr("Composite DataSet successfully imported"),
		     3000));
    }

    return success;
}


void medToolBoxCompositeDataSetImporter::onAddClicked()
{
  if (!d->type->currentIndex())
  {
      emit (showError(this,
		     tr("Please select a type first"),
		     3000));
      return;
  }
  
    
    QStringList filenames = QFileDialog::getOpenFileNames( 
        this, 
        tr("Open File(s)"), 
        QDir::currentPath(), 
        tr("All files (*.*)") );
    if( !filenames.isEmpty() )
    {
      qDebug( filenames.join(",").toAscii() );
    }
}


void medToolBoxCompositeDataSetImporter::onCurrentTypeChanged(int index)
{
  if (!index)
    return;

  
}

