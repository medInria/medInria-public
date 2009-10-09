/* medPatientPreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:46:53 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct  9 16:02:47 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 147
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medPatientPreview.h"

#include <dtkCore/dtkGlobal.h>

#include <medSql/medDatabaseController.h>

#include <medGui/medImageReflector.h>
#include <medGui/medImageStack.h>

class medPatientPreviewPrivate
{
public:
    QLabel *info_name;

    medImageReflector *avatar;
    medImageStack *stack;
};

medPatientPreview::medPatientPreview(QWidget *parent) : QWidget(parent), d(new medPatientPreviewPrivate)
{
    d->avatar = new medImageReflector(this);
    d->avatar->setImage(QImage(":/img/unknown.jpg"));

    d->stack = new medImageStack(this);

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

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(d->avatar);
    layout->addWidget(information);
    layout->addWidget(d->stack);
}

medPatientPreview::~medPatientPreview(void)
{
    delete d;

    d = NULL;
}

QSize medPatientPreview::sizeHint(void) const
{
    return d->avatar->sizeHint();
}

void medPatientPreview::setup(int patientId)
{
    d->stack->clear();

    QSqlQuery query(*(medDatabaseController::instance()->database())); QVariant id;
    
    // Retrieve patient information

    QString patientName;

    query.prepare("SELECT name FROM patient WHERE id = :id");
    query.bindValue(":id", patientId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;
    
    if(query.first()) {
        patientName = query.value(0).toString();
    }

    // Retrieve studies information

    QList<QVariant> studyIds;

    query.prepare("SELECT id FROM study WHERE patient = :patient");
    query.bindValue(":patient", patientId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;
    
    while(query.next()) {
        studyIds << query.value(0);
    }

    // Retrieve series information

    int stackCount = 0;

    foreach(QVariant study, studyIds) {

    int seriesCount = 0;

    query.prepare("SELECT id FROM series WHERE study = :study");
    query.bindValue(":study", study);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    while(query.next())
        seriesCount++;
    
    d->stack->setStackSize(stackCount++, seriesCount);

    }

    // Build visual

    d->info_name->setText(patientName);

    this->update();
}

void medPatientPreview::paintEvent(QPaintEvent *event)
{
    QRadialGradient gradient;
    gradient.setCenter(event->rect().center().x(), event->rect().center().y()+3*event->rect().height()/2);
    gradient.setFocalPoint(event->rect().center());
    gradient.setRadius(event->rect().width()/3);
    gradient.setColorAt(0, QColor(0x91, 0x91, 0x91));
    gradient.setColorAt(1, QColor(0x49, 0x49, 0x49));

    QPainter painter(this);
    painter.fillRect(event->rect(), gradient);

    QWidget::paintEvent(event);
}
