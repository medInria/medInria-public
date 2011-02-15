#include "medStartupSettingsWidget.h"
#include <QWidget>
#include <QtGui>
#include <QtCore>

#include <medCore/medSettingsManager.h>

class medStartupSettingsWidgetPrivate {

public:
  QWidget* parent;
  QCheckBox* startInFullScreen;

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
    QFormLayout* layout = new QFormLayout;
    layout->addRow(tr("Shall medINRIA start in full screen?"),d->startInFullScreen);
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
}

bool medStartupSettingsWidget::write()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    mnger->setValue("startup","start_in_full_screen", d->startInFullScreen->isChecked());
    return true;
}
