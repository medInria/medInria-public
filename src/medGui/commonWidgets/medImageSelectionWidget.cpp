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

    QList<medDataIndex> selectedImages;

    QPushButton* btOk;
    QPushButton* btCancel;

//    QTreeView* treeView;
};

medImageSelectionWidget::medImageSelectionWidget(QList<medDataIndex> indexes, QWidget *parent) : d(new medImageSelectionWidgetPrivate)
{
    d->selectedImages = indexes;

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


    d->btOk = new QPushButton(tr("OK"), this);
    d->btCancel = new QPushButton(tr("Cancel"), this);
    d->btOk->setDefault( true );

    QHBoxLayout* buttonlayout = new QHBoxLayout;
    buttonlayout->addWidget( d->btCancel );
    buttonlayout->addWidget( d->btOk );

    QVBoxLayout* vlayout = new QVBoxLayout(displayWidget);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(d->selected);
    vlayout->addLayout(buttonlayout);
//    vlayout->addWidget(new medDropSite());

    connect(d->view, SIGNAL(patientClicked(const medDataIndex&)), this, SLOT(onPatientSelected(const medDataIndex&)));
    connect(d->view, SIGNAL(seriesClicked(const medDataIndex&)), d->preview, SLOT(addSeriesItem(const medDataIndex&)));

    connect(d->btOk, SIGNAL(clicked()), this, SLOT(onOkClicked()));
    connect(d->btCancel, SIGNAL(clicked()), this, SLOT(onCancelClicked()));

//    connect(d->view, SIGNAL(studyClicked(const medDataIndex&)), d->selected, SLOT(onStudyClicked(const medDataIndex&)));

    // for the moment we just need patient and study
    for (int var = 2; var < d->proxy->columnCount(); ++var) {
//        d->treeView->hideColumn(var);
        d->view->hideColumn(var);
    }

    foreach(medDataIndex ind, indexes)
    {
        d->selected->addSeriesItem(ind);
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
    d->preview->reset();

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

void medImageSelectionWidget::onCancelClicked()
{
    qDebug() << "canceeeeeeeeeeeeeeeeeeeeeeel";
    reject();
}

void medImageSelectionWidget::onOkClicked()
{
    qDebug() << "oooooooooooooKKKKKKKKKKKKKKKKKK";
    accept();
}

QList<medDataIndex> medImageSelectionWidget::getSelectedIndexes()
{
    return d->selected->getContainedIndexes();
}
