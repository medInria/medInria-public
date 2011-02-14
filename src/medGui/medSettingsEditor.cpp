#include "medSettingsEditor.h"


#include <QtGui>
#include <QWidget>

#include <medGui/medSystemSettingsWidget.h>
#include <medGui/medSettingsWidgetFactory.h>

class medSettingsEditorPrivate {

public:
  QWidget* parent;
  QTabWidget* tabWidget;
  medSettingsWidget * systemSettingsWidget;

  medSettingsEditorPrivate();
  ~medSettingsEditorPrivate();
};

medSettingsEditorPrivate::medSettingsEditorPrivate()
{
}

medSettingsEditorPrivate::~medSettingsEditorPrivate()
{
}

medSettingsEditor::medSettingsEditor(QWidget *parent) :
    QWidget(parent),d(new medSettingsEditorPrivate())
{
  d->tabWidget = new QTabWidget (this);
  d->tabWidget->setTabPosition(QTabWidget::West);

  //System widget
  d->systemSettingsWidget = medSettingsWidgetFactory::instance()->createSettingsWidget("System",this);
  //Dicom widget
  //Startup widget
  //Logging widget
  //Process manager widget

  d->tabWidget->addTab(d->systemSettingsWidget,
                       d->systemSettingsWidget->section());
}
