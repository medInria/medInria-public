#include "medSystemSettingsWidget.h"
#include <QWidget>


class medSystemSettingsWidgetPrivate {

public:
  QWidget* Parent;

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

}
