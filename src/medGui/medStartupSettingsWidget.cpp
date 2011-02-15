#include "medStartupSettingsWidget.h"
#include <QWidget>
#include <QtGui>
#include <QtCore>

#include <medCore/medSettingsManager.h>

class medStartupSettingsWidgetPrivate {

public:
  QWidget* parent;
  QCheckBox* startInFullScreen;
  QComboBox* defaultStartingArea;

  medStartupSettingsWidgetPrivate();
  ~medStartupSettingsWidgetPrivate();
};

medStartupSettingsWidgetPrivate::medStartupSettingsWidgetPrivate()
{
}

medStartupSettingsWidgetPrivate::~medStartupSettingsWidgetPrivate()
{
}


medStartupSettingsWidget::medStartupSettingsWidget(QWidget *parent) :
        medSettingsWidget(parent),
        d(new medStartupSettingsWidgetPrivate())
{
    setSection("Startup");
    d->startInFullScreen = new QCheckBox(this);

    d->defaultStartingArea = new QComboBox();
    d->defaultStartingArea->addItem("Browser", "Browser");
    d->defaultStartingArea->addItem("Viewer", "Viewer");

    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Shall medINRIA start in full screen?"),d->startInFullScreen);
    layout->addRow(tr("Area selected on startup:"),d->defaultStartingArea);
    this->setLayout(layout);
}


medSettingsWidget* createStartupSettingsWidget(QWidget *parent){
    return new medStartupSettingsWidget(parent);
}

bool medStartupSettingsWidget::validate()
{
    return true;
}

void medStartupSettingsWidget::read()
{
    qDebug()<<"reading QSettings";
    medSettingsManager * mnger = medSettingsManager::instance();
    d->startInFullScreen->setChecked(mnger->value("startup", "start_in_full_screen").toBool());

	QVariant vArea = mnger->value("startup", "default_starting_area");
	//if nothing is configured then Browser is the default area
	int areaIndex = 0;
	if (!vArea.isNull())
		areaIndex = d->defaultStartingArea->findText(vArea.toString());

	//in case nothing is found, ensure to select a proper index
	if (areaIndex == -1)
		areaIndex = 0;

	d->defaultStartingArea->setCurrentIndex(areaIndex);
}

bool medStartupSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("startup","start_in_full_screen", d->startInFullScreen->isChecked());
    mnger->setValue("startup","default_starting_area", d->defaultStartingArea->itemText(d->defaultStartingArea->currentIndex()));
    return true;
}
