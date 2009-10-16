/* medStudyPreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:47:21 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct 16 13:09:46 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 80
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medStudyPreview.h"

#include <dtkCore/dtkGlobal.h>

#include <medSql/medDatabaseController.h>

#include <medGui/medImageReflector.h>
#include <medGui/medImageList.h>

#include <QtSql>

class medStudyPreviewPrivate
{
public:
    QLabel *info_name;

    medImageReflector *avatar;
    medImageList *list;
};

medStudyPreview::medStudyPreview(QWidget *parent) : QWidget(parent), d(new medStudyPreviewPrivate)
{
    QWidget *main = new QWidget(this);

    d->avatar = new medImageReflector(this);
    d->avatar->setImage(QImage(":/img/unknown.jpg"));

    d->list = new medImageList(this);

    d->info_name = new QLabel(this);

    QFormLayout *form_layout = new QFormLayout;
    form_layout->addRow("Patient last name:", d->info_name);
    form_layout->addRow("Patient first name:", new QLabel("Prout"));
    form_layout->addRow("Information 3:", new QLabel("Prout"));
    form_layout->addRow("Information 4:", new QLabel("Prout"));
    form_layout->addRow("Information 5:", new QLabel("Prout"));
    form_layout->addRow("Information 6:", new QLabel("Prout"));

    QWidget *information = new QWidget(this);
    information->setLayout(form_layout);
    information->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d->avatar);
    layout->addWidget(information);
    layout->addWidget(d->list);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

medStudyPreview::~medStudyPreview(void)
{
    delete d;

    d = NULL;
}

QSize medStudyPreview::sizeHint(void) const
{
    return d->avatar->sizeHint();
}

void medStudyPreview::setup(int studyId)
{
    d->list->clear();

    QSqlQuery query(*(medDatabaseController::instance()->database())); QVariant id;
    
    // Retrieve study information

    QVariant studyPatient;
    QVariant studyName;

    query.prepare("SELECT patient, name FROM study WHERE id = :id");
    query.bindValue(":id", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;
    
    if (query.first()) {
        studyPatient = query.value(0);
        studyName = query.value(1);
    }

    // Retrieve patient information

    QString patientName;

    query.prepare("SELECT name FROM patient WHERE id = :id");
    query.bindValue(":id", studyPatient);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;
    
    if(query.first()) {
        patientName = query.value(0).toString();
    }

    // Retrieve series information

    int seriesCount = 0;

    QList<QVariant> seriesIds;
    QList<QVariant> seriesNames;
    QList<QVariant> seriesSizes;

    query.prepare("SELECT id, name, size FROM series WHERE study = :study");
    query.bindValue(":study", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    while(query.next()) {
        seriesIds << query.value(0);
        seriesNames << query.value(1);
        seriesSizes << query.value(2);
        seriesCount++;
    }

    for(int i = 0; i < seriesCount; i++) {
        d->list->setListName(i, seriesNames.at(i).toString());
        d->list->setListSize(i, seriesSizes.at(i).toInt());
    }

    // Build visual

    d->info_name->setText(patientName);

    this->update();
}

void medStudyPreview::paintEvent(QPaintEvent *event)
{
    QLinearGradient gradient;
    gradient.setStart(event->rect().topLeft());
    gradient.setFinalStop(event->rect().bottomLeft());

    gradient.setColorAt(0.0, QColor(0x49, 0x49, 0x49));
    gradient.setColorAt(1.0, QColor(0x31, 0x31, 0x31));

    QPainter painter(this);
    painter.fillRect(event->rect(), gradient);
    painter.end();
}
