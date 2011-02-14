#include "medSettingsWidget.h"

#include <QWidget>
#include <medCore/medMessageController.h>


class medSettingsWidgetPrivate {

public:
    QWidget* Parent;
    QString section;
    medSettingsWidgetPrivate(const QString& section);
    ~medSettingsWidgetPrivate();
};

medSettingsWidgetPrivate::medSettingsWidgetPrivate(const QString& section)
{
    this->section = QString(section);
}

medSettingsWidgetPrivate::~medSettingsWidgetPrivate()
{
}

medSettingsWidget::medSettingsWidget(const QString& section,
                                     QWidget *parent) : QWidget(parent), d(new medSettingsWidgetPrivate(section))
{
    connect(this,SIGNAL(showError(QObject*,const        QString&,unsigned int)),
        medMessageController::instance(),SLOT(showError (QObject*,const QString&,unsigned int)));

}


bool medSettingsWidget::validate()
{
    return false;
}

bool medSettingsWidget::save()
{
    if (!validate())
    {
        QString error = "Error in validation of section";
        error.append(d->section);
        emit (showError(this,error,3));
        return false;
    }
    else
    {
        //do the saving
        return true;
    }
}
