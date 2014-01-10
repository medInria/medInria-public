#include "medDatabaseCompactWidget.h"

#include "medDatabaseView.h"
#include "medDatabasePreview.h"

#include <QGridLayout>


class medDatabaseCompactWidgetPrivate
{
public:
    medDatabasePreview *preview;
    medDatabaseView *view;
    QGridLayout *layout;
};


medDatabaseCompactWidget::medDatabaseCompactWidget(QWidget *parent): d(new medDatabaseCompactWidgetPrivate)
{
    d->layout = new QGridLayout(this);
    this->setLayout(d->layout);
}


medDatabaseCompactWidget::~medDatabaseCompactWidget()
{
    delete d;
    d = NULL;
}


void medDatabaseCompactWidget::resizeEvent(QResizeEvent *event)
{
    delete d->layout;
    d->layout = new QGridLayout(this);
    this->setLayout(d->layout);

    d->layout->addWidget(d->view, 0,0,1,1);
    d->layout->addWidget(d->preview, 1,0,1,1,Qt::AlignBottom);

    QWidget::resizeEvent(event);
}

void medDatabaseCompactWidget::setView(medDatabaseView *view)
{
    d->view = view;
    d->layout->addWidget(d->view,0,0,1,1);
}

void
medDatabaseCompactWidget::setPreview(medDatabasePreview *preview)
{
    d->preview = preview;
    d->layout->addWidget(d->preview, 1,0,1,1,Qt::AlignBottom);
}
