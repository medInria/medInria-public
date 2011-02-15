#include "medStartupSettingsWidget.h"
#include <QWidget>
#include <QtGui>
#include <QtCore>


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
    return false;
}

void medStartupSettingsWidget::read()
{
    qDebug()<<"reading QSettings";
}
