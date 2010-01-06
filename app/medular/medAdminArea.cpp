/* medAdminArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:36:45 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:36:48 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include    "medAdminArea.h"
#include "ui_medAdminArea.h"

#include <dtkGui/dtkSettingsEditor.h>

class medAdminAreaPrivate
{
public:
    Ui::medAdminArea *ui;

    dtkSettingsEditor *settings_editor;
};

medAdminArea::medAdminArea(QWidget *parent) : QWidget(parent), d(new medAdminAreaPrivate)
{
    d->settings_editor = new dtkSettingsEditor(this);
    d->settings_editor->setSettings("inria", "dtk");

    d->ui = new Ui::medAdminArea;
    d->ui->setupUi(this);
    d->ui->tab->addTab(d->settings_editor, "Settings");

    connect(d->ui->buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
    connect(d->ui->buttonBox, SIGNAL(rejected()), this, SIGNAL(rejected()));
}

medAdminArea::~medAdminArea(void)
{
    delete d->ui;
    delete d;

    d = NULL;
}

void medAdminArea::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        d->ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
