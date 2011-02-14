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

medSettingsWidget::medSettingsWidget(QWidget *parent) :
    QWidget(parent),d(new medSettingsWidgetPrivate())
{
  d->tabWidget = new QTabWidget (this);
  d->tabWidget->setTabPosition(QTabWidget::West);

  //System widget
  d->systemSettingsWidget = new medSystemSettingsWidget();
  //Dicom widget
  //Startup widget
  //Logging widget
  //Process manager widget


  d->tabWidget->addTab(d->SystemSettingsWidget,tr("System"));
}
