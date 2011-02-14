#include "medSystemSettingsWidget.h"
#include <QWidget>
#include <QtGui>


class medSystemSettingsWidgetPrivate {

public:
  QWidget* parent;
  QLineEdit* pluginPathField;

  medSystemSettingsWidgetPrivate();
  ~medSystemSettingsWidgetPrivate();
};

medSystemSettingsWidgetPrivate::medSystemSettingsWidgetPrivate()
{
}

medSystemSettingsWidgetPrivate::~medSystemSettingsWidgetPrivate()
{
}


medSystemSettingsWidget::medSystemSettingsWidget(QWidget *parent) : medSettingsWidget(parent), d(new medSystemSettingsWidgetPrivate())
{
    d->pluginPathField = new QLineEdit(this);
    setSection("System");
    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Plugin Path"),d->pluginPathField);
}


medSettingsWidget* createSystemSettingsWidget(QWidget *parent){
    return new medSystemSettingsWidget(parent);
}
