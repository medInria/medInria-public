#include "medImageSelectionWidget.h"

#include <QtGui>

#include <medDataManager.h>
#include <medDatabaseView.h>
#include <medDatabasePreview.h>
#include <medDatabaseProxyModel.h>
#include <medDatabaseModel.h>
#include <medDatabaseExporter.h>


class medImageSelectionWidgetPrivate
{
public:
    QWidget* selectedSeriesWidget;

    medDatabasePreview *preview;
    medDatabaseModel *model;
    medDatabaseView *view;
    medDatabaseProxyModel *proxy;

//    QTreeView* treeView;
};

medImageSelectionWidget::medImageSelectionWidget(QWidget *parent) : d(new medImageSelectionWidgetPrivate)
{
    QWidget* displayWidget = new QWidget(this);
    displayWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->model = new medDatabaseModel(this, true);
    d->proxy = new medDatabaseProxyModel(this);
    d->proxy->setSourceModel(d->model);
    d->preview = new medDatabasePreview(displayWidget);

    QSizePolicy* policy = new QSizePolicy();
//    policy->setHorizontalStretch(10);
    policy->setHorizontalPolicy(QSizePolicy::Expanding);
    d->preview->setSizePolicy(*policy);
    d->preview->setMinimumSize(QSize(600, 400));

    d->view    = new medDatabaseView(displayWidget);
    d->view->setModel(d->proxy);
//    d->treeView = new QTreeView(displayWidget);
//    d->treeView->setModel(d->proxy);

    QHBoxLayout *hlayout = new QHBoxLayout();
//    hlayout->addWidget(d->treeView);
    hlayout->addWidget(d->view);
    hlayout->addWidget(d->preview);

    d->selectedSeriesWidget = new QWidget(displayWidget);

    QVBoxLayout* vlayout = new QVBoxLayout(displayWidget);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(d->selectedSeriesWidget);

    connect(d->view, SIGNAL(patientClicked(const medDataIndex&)), d->preview, SLOT(onPatientClicked(const medDataIndex&)));
    connect(d->view, SIGNAL(studyClicked(const medDataIndex&)), d->preview, SLOT(onStudyClicked(const medDataIndex&)));
    connect(d->view, SIGNAL(seriesClicked(const medDataIndex&)), d->preview, SLOT(onSeriesClicked(const medDataIndex&)));

    QLabel* label = new QLabel("PLACEHOLDER", d->selectedSeriesWidget);

    // for the moment we just need patient and study
    for (int var = 2; var < d->proxy->columnCount(); ++var) {
//        d->treeView->hideColumn(var);
        d->view->hideColumn(var);
    }
}

medImageSelectionWidget::~medImageSelectionWidget(void)
{
    delete d;

    d = NULL;
}

QSize medImageSelectionWidget::sizeHint(void) const
{
    return QSize(1000, 1200);
}

void medImageSelectionWidget::clear()
{
}
