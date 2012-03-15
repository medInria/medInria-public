#include "medImageSelectionWidget.h"

#include <QtGui>

#include <medDataManager.h>
#include <medDatabaseView.h>
#include <medDatabasePreview.h>
#include <medDatabaseProxyModel.h>
#include <medDatabaseModel.h>
#include <medDatabaseExporter.h>
#include <medAbstractDbController.h>

#include <medDropSite.h>
#include <medThumbnailContainer.h>

class medImageSelectionWidgetPrivate
{
public:
    // data
    medDatabaseModel *model;
    medDatabaseProxyModel *proxy;

    // view
    medThumbnailContainer *preview;
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

    d->preview = new medThumbnailContainer(displayWidget);
    d->preview->setAllowDeleting(false);
    d->preview->setAllowDragging(true);
    d->preview->setAllowDropping(false);
    d->preview->setColumnsCount(4);
    d->preview->setRowsCount(2);

    d->selected = new medThumbnailContainer(displayWidget);
    d->selected->setAllowDeleting(true);
    d->selected->setAllowDragging(false);
    d->selected->setAllowDropping(true);
    d->selected->setColumnsCount(5);
    d->selected->setRowsCount(3);

    QSizePolicy* policy = new QSizePolicy();
//    policy->setHorizontalStretch(10);
    policy->setHorizontalPolicy(QSizePolicy::Expanding);
//    d->preview->setSizePolicy(*policy);
//    d->preview->setMinimumSize(QSize(600, 400));

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

    connect(d->view, SIGNAL(patientClicked(const medDataIndex&)), this, SLOT(onPatientSelected(const medDataIndex&)));
    connect(d->view, SIGNAL(seriesClicked(const medDataIndex&)), d->preview, SLOT(addSeriesItem(const medDataIndex&)));

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

void medImageSelectionWidget::onPatientSelected(const medDataIndex& id)
{
    medAbstractDbController * db =  medDataManager::instance()->controllerForDataSource(id.dataSourceId());
    if ( db ) {

        QList<medDataIndex> studies = db->studies(id);
        for (QList<medDataIndex>::const_iterator studyIt( studies.begin() ); studyIt != studies.end(); ++studyIt ) {

            QList<medDataIndex> series = db->series(*studyIt);
            for (QList<medDataIndex>::const_iterator seriesIt( series.begin() ); seriesIt != series.end(); ++seriesIt ) {

                d->preview->addSeriesItem(*seriesIt);
            }
        }
    }
}
