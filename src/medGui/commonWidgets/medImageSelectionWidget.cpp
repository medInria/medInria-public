#include "medImageSelectionWidget.h"

#include <QtGui>

#include <medDataManager.h>
#include <medDatabaseView.h>
#include <medDatabasePreview.h>
#include <medDatabaseProxyModel.h>
#include <medDatabaseModel.h>
#include <medDatabaseExporter.h>

#include <medDropSite.h>
#include <medThumbnailContainer.h>


class medImageSelectionWidgetPrivate
{
public:
    // data
    medDatabaseModel *model;
    medDatabaseProxyModel *proxy;

    // view
    medDatabasePreview *preview;
    medDatabaseView *view;
    medThumbnailContainer *selected;

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

    d->selected = new medThumbnailContainer(displayWidget);

    QSizePolicy* policy = new QSizePolicy();
//    policy->setHorizontalStretch(10);
    policy->setHorizontalPolicy(QSizePolicy::Expanding);
    d->preview->setSizePolicy(*policy);
    d->preview->setMinimumSize(QSize(600, 400));

    d->selected->setMinimumSize(QSize(600 + 100, 300));

    d->view    = new medDatabaseView(displayWidget);
    d->view->setModel(d->proxy);
//    d->treeView = new QTreeView(displayWidget);
//    d->treeView->setModel(d->proxy);

    QHBoxLayout *hlayout = new QHBoxLayout();
//    hlayout->addWidget(d->treeView);
    hlayout->addWidget(d->view);
    hlayout->addWidget(d->preview);



    QVBoxLayout* vlayout = new QVBoxLayout(displayWidget);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(d->selected);
//    vlayout->addWidget(new medDropSite());

    connect(d->view, SIGNAL(patientClicked(const medDataIndex&)), d->preview, SLOT(onPatientClicked(const medDataIndex&)));
    connect(d->view, SIGNAL(studyClicked(const medDataIndex&)), d->preview, SLOT(onStudyClicked(const medDataIndex&)));
    connect(d->view, SIGNAL(seriesClicked(const medDataIndex&)), d->preview, SLOT(onSeriesClicked(const medDataIndex&)));

//    connect(d->view, SIGNAL(studyClicked(const medDataIndex&)), d->selected, SLOT(onStudyClicked(const medDataIndex&)));

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
