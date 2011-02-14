#include "medSettingsWidget.h"
#include <QWidget>


class medSettingsWidgetPrivate {

public:
  QWidget* Parent;

  medSettingsWidgetPrivate();
  ~medSettingsWidgetPrivate();
};

medSettingsWidgetPrivate::medSettingsWidgetPrivate()
{
}

medSettingsWidgetPrivate::~medSettingsWidgetPrivate()
{
}

medSettingsWidget::medSettingsWidget(QWidget *parent) : QWidget(parent), d(new medSettingsWidgetPrivate())
{

}
