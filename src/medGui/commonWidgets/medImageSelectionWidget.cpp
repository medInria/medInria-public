#include "medImageSelectionWidget.h"

#include <QtGui>

class medImageSelectionWidgetPrivate
{
public:
    QWidget* patientsWidget;
    QWidget* studiesWidget;
    QWidget* selectedStudiesWidget;
};

medImageSelectionWidget::medImageSelectionWidget(QWidget *parent) : d(new medImageSelectionWidgetPrivate)
{
    QWidget* displayWidget = new QWidget();

    d->patientsWidget = new QWidget(displayWidget);
    d->studiesWidget = new QWidget(displayWidget);
    d->selectedStudiesWidget = new QWidget(displayWidget);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(d->patientsWidget);
    hlayout->addWidget(d->studiesWidget);

    QVBoxLayout* vlayout = new QVBoxLayout(this);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(d->selectedStudiesWidget);

    // test

    QLabel* lpatients = new QLabel("patients", d->patientsWidget);
    QLabel* lstudies = new QLabel("studies", d->studiesWidget);
    QLabel* lselection = new QLabel("selection", d->selectedStudiesWidget);
}

medImageSelectionWidget::~medImageSelectionWidget(void)
{
    delete d;

    d = NULL;
}

QSize medImageSelectionWidget::sizeHint(void) const
{
    return QSize(500, 500);
}

void medImageSelectionWidget::clear()
{
}
