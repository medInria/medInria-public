#include "medSystemSettingsWidget.h"
#include <QWidget>

class medSystemSettingsWidgetPrivate {

public:
  QWidget* Parent;
  QLineEdit * pluginPathField;

  medSystemSettingsWidgetPrivate();
  ~medSystemSettingsWidgetPrivate();
};

medSystemSettingsWidgetPrivate::medSystemSettingsWidgetPrivate()
{
}

medSystemSettingsWidgetPrivate::~medSystemSettingsWidgetPrivate()
{
}

medSystemSettingsWidget::medSystemSettingsWidget(QWidget *parent) : QWidget(parent), d(new medSystemSettingsWidgetPrivate())
{
	d->pluginPathField = new QLineEdit(this);

	QFormLayout* layout = new QFormLayout;
	layout->addRow(tr("Plugin Path"),d->pluginPathField);
}
