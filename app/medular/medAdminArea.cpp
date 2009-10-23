#include    "medAdminArea.h"
#include "ui_medAdminArea.h"

class medAdminAreaPrivate
{
public:
    Ui::medAdminArea *ui;
};

medAdminArea::medAdminArea(QWidget *parent) : QWidget(parent), d(new medAdminAreaPrivate)
{
    d->ui = new Ui::medAdminArea;
    d->ui->setupUi(this);

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
