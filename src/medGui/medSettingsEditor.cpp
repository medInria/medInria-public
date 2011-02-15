#include "medSettingsEditor.h"


#include <QtGui>
#include <QWidget>

#include <medGui/medSystemSettingsWidget.h>
#include <medGui/medSettingsWidgetFactory.h>

class medSettingsEditorPrivate {

public:
  QWidget* parent;
  QTabWidget* tabWidget;
  QList<medSettingsWidget*> settingsWidgets;
  //medSettingsWidget * systemSettingsWidget;

  medSettingsEditorPrivate();
  ~medSettingsEditorPrivate();
};

medSettingsEditorPrivate::medSettingsEditorPrivate()
{
}

medSettingsEditorPrivate::~medSettingsEditorPrivate()
{
    foreach(medSettingsWidget* widget, settingsWidgets)
    {
        delete widget;
    }
}

medSettingsEditor::medSettingsEditor(QWidget *parent) :
    QWidget(parent),d(new medSettingsEditorPrivate())
{
    QVBoxLayout * vLayout = new QVBoxLayout(this);
    setLayout(vLayout);
    this->setFixedSize(QSize(300,300));
  d->tabWidget = new QTabWidget (this);
  d->tabWidget->setTabPosition(QTabWidget::West);
  d->tabWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  //System widget
  medSettingsWidget * systemSettingsWidget = medSettingsWidgetFactory::instance()->createSettingsWidget("System",NULL);

  d->tabWidget->addTab(systemSettingsWidget,
                       systemSettingsWidget->section());
  d->settingsWidgets.append(systemSettingsWidget);
  this->layout()->addWidget(d->tabWidget);
  //Dicom widget
  //Startup widget
  //Logging widget
  //Process manager widget

  //save button
  QPushButton * save = new QPushButton (tr("Save"),this);
  connect(save,SIGNAL(clicked()),
          this,SLOT(onSaveClicked()));

  //cancel button
  QPushButton * cancel = new QPushButton (tr("Cancel"),this);
  connect(cancel,SIGNAL(clicked()),
          this,SLOT(onCancelClicked()));
  //reset button
  QPushButton * reset = new QPushButton (tr("Reset"), this);
  connect(reset,SIGNAL(clicked()),
          this,SLOT(onResetClicked()));

  QHBoxLayout * buttonLayout = new QHBoxLayout(this);
  buttonLayout->addWidget(reset);
  buttonLayout->addWidget(cancel);
  buttonLayout->addWidget(save);
  vLayout->addLayout(buttonLayout);
}


void medSettingsEditor::onSaveClicked(){
    foreach(medSettingsWidget * setting, d->settingsWidgets)
    {
        //maybe call validate (and make it public) instead
        //to avoid half saved solutions, or keep on saving others?
        if (!setting->save())
            break;
    }
}

void medSettingsEditor::onCancelClicked(){
    this->close();
}

void medSettingsEditor::onResetClicked(){
    foreach(medSettingsWidget * setting, d->settingsWidgets)
    {
        setting->read();
    }
}
