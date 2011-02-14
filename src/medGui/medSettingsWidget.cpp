#include "medSettingsWidget.h"

#include <QWidget>
#include <medCore/medMessageController.h>


class medSettingsWidgetPrivate {

public:
    QWidget* Parent;
    QString section;
    medSettingsWidgetPrivate();
    ~medSettingsWidgetPrivate();
};

medSettingsWidgetPrivate::medSettingsWidgetPrivate()
{
    this->section = QString();
}

medSettingsWidgetPrivate::~medSettingsWidgetPrivate()
{
}

medSettingsWidget::medSettingsWidget(QWidget *parent) : QWidget(parent), d(new medSettingsWidgetPrivate())
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

void medSettingsWidget::setSection(QString section)
{
    d->section = section;
}

const QString& medSettingsWidget::section()const
{
    return d->section;
}
