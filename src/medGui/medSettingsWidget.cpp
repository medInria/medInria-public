#include "medSettingsWidget.h"

#include <QWidget>
#include <QtGui>


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
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}


bool medSettingsWidget::validate()
{
    return false;
}

void medSettingsWidget::setSection(QString section)
{
    d->section = section;
}

const QString& medSettingsWidget::section()const
{
    return d->section;
}

void medSettingsWidget::read()
{
    qDebug() << "read QSettings";
}

