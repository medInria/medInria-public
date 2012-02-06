#include "medStartupSettingsWidget.h"
#include <QWidget>
#include <QtGui>
#include <QtCore>

#include <medSettingsManager.h>

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
    setTabName(tr("Startup"));
    d->startInFullScreen = new QCheckBox(this);
    d->startInFullScreen->setToolTip(tr("Start application in full screen mode?"));

    d->defaultStartingArea = new QComboBox(this);
    d->defaultStartingArea->addItem(tr("Homepage"));
    d->defaultStartingArea->addItem(tr("Browser"));
    d->defaultStartingArea->addItem(tr("Viewer"));

    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Fullscreen"),d->startInFullScreen);
    layout->addRow(tr("Starting area"), d->defaultStartingArea);
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
    medSettingsManager * mnger = medSettingsManager::instance();
    d->startInFullScreen->setChecked(mnger->value("startup", "fullscreen").toBool());

    //if nothing is configured then Browser is the default area
    int areaIndex = mnger->value("startup", "default_starting_area", 0).toInt();

    // clamp range
    if (areaIndex < 0)
        areaIndex = 0;
    if (areaIndex > d->defaultStartingArea->count() -1)
        areaIndex = d->defaultStartingArea->count() -1;

    d->defaultStartingArea->setCurrentIndex(areaIndex);
}

bool medStartupSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("startup","fullscreen", d->startInFullScreen->isChecked());
    mnger->setValue("startup","default_starting_area", d->defaultStartingArea->currentIndex());
    return true;
}

QString medStartupSettingsWidget::identifier() const {
    return "startup";
}

QString medStartupSettingsWidget::description() const {
    return "startup";
}
