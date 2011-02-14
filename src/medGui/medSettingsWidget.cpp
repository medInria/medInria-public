#include "medSettingsWidget.h"
#include <QtGui>


class medSettingsWidgetPrivate {
public:
  QWidget* Parent;
  QTabWidget* tabWidget;
  //QWidget * SystemWidget;
  QLineEdit * pluginPathField;

  medSettingsWidgetPrivate();
  ~medSettingsWidgetPrivate();
};

medSettingsWidgetPrivate::medSettingsWidgetPrivate()
{
}


medSettingsWidgetPrivate::~medSettingsWidgetPrivate()
{
}

medSettingsWidget::medSettingsWidget(QWidget *parent) :
    QWidget(parent),d(new medSettingsWidgetPrivate())
{
  d->tabWidget = new QTabWidget (this);
  d->tabWidget->setTabPosition(QTabWidget::West);

  //System Widget
  createSystemWidget();
  //Dicom Widget
  //startup widget
  //Logging widget
  //process manager widget


  //d->tabWidget->addTab(d->SystemWidget,tr("System"));
}


void medSettingsWidget::createSystemWidget()
{


  QWidget * SystemWidget = new QWidget(this);
  d->pluginPathField = new QLineEdit(SystemWidget);

  QFormLayout* layout = new QFormLayout;
  layout->addRow(tr("Plugin Path"),d->pluginPathField);
  d->tabWidget->addTab(SystemWidget,tr("System"));
}
