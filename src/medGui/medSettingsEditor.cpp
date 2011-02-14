#include "medSettingsEditor.h"
#include "medSystemSettingsWidget.h"
#include <QtGui>
#include <QWidget>


class medSettingsEditorPrivate {

public:
  QWidget* parent;
  QTabWidget* tabWidget;
  QWidget * systemSettingsWidget;

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
  d->systemSettingsWidget = new medSystemSettingsWidget();
  //Dicom widget
  //Startup widget
  //Logging widget
  //Process manager widget


  d->tabWidget->addTab(d->systemSettingsWidget,tr("System"));
}
