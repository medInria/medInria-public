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

medSystemSettingsWidget* medSystemSettingsWidget::createSystemSettingsWidget(QWidget *parent){
	return new medSystemSettingsWidget(parent);
}

medSystemSettingsWidget::medSystemSettingsWidget(QWidget *parent) : medSettingsWidget(parent), d(new medSystemSettingsWidgetPrivate())
{
	d->pluginPathField = new QLineEdit(this);

	QFormLayout* layout = new QFormLayout;
	layout->addRow(tr("Plugin Path"),d->pluginPathField);
}
