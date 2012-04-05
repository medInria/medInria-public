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
};

medImageSelectionWidget::medImageSelectionWidget(QList<medDataIndex> indexes, QWidget *parent) : d(new medImageSelectionWidgetPrivate)
{
    d->selectedImages = indexes;

    d->model = new medDatabaseModel(this, true);
    d->proxy = new medDatabaseProxyModel(this);
    d->proxy->setSourceModel(d->model);

    d->preview = new medThumbnailContainer();
    d->preview->setAllowDeleting(false);
    d->preview->setAllowDragging(true);
    d->preview->setAllowDropping(false);
    d->preview->setColumnsCount(4);
    d->preview->setRowsCount(2);

    d->selected = new medThumbnailContainer();
    d->selected->setAllowDeleting(true);
    d->selected->setAllowDragging(false);
    d->selected->setAllowDropping(true);
    d->selected->setColumnsCount(6);
    d->selected->setRowsCount(3);

    d->view = new medDatabaseView();
    d->view->setModel(d->proxy);
    d->view->setColumnWidth(0, 120);

    d->btOk = new QPushButton(tr("OK"));
    d->btCancel = new QPushButton(tr("Cancel"));
    d->btOk->setDefault( true );

    // layouting

    QVBoxLayout* main_layout = new QVBoxLayout();

    QSplitter* splitterHorizontal = new QSplitter(Qt::Horizontal);
    QSplitter* splitterVertical = new QSplitter(Qt::Vertical);
    splitterHorizontal->setHandleWidth(1);
    splitterVertical->setHandleWidth(1);
	splitterHorizontal->setOpaqueResize(false);
	splitterVertical->setOpaqueResize(false);

    splitterHorizontal->insertWidget(0, d->view);
    splitterHorizontal->insertWidget(1, d->preview);

	QList<int> sizes;
	sizes << 240 << 550;
	splitterHorizontal->setSizes(sizes);

	splitterHorizontal->setStretchFactor(1, 1);

    splitterVertical->insertWidget(0, splitterHorizontal);
    splitterVertical->insertWidget(1, d->selected);

    QHBoxLayout* btl = new QHBoxLayout();
    btl->setDirection(QBoxLayout::RightToLeft);

//    btl->addWidget(d->btCancel, 0, Qt::AlignRight);
//    btl->addWidget(d->btOk, 0, Qt::Align);
    d->btOk->setFixedSize(60, 35);
    d->btCancel->setFixedSize(60, 35);
    btl->addWidget(d->btOk);
    btl->addWidget(d->btCancel);

    main_layout->addWidget(splitterVertical);
    main_layout->addLayout(btl);

    this->setLayout(main_layout);
//    d->view->setParent(this);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(d->view, SIGNAL(patientClicked(const medDataIndex&)), this, SLOT(onPatientSelected(const medDataIndex&)));
    connect(d->view, SIGNAL(seriesClicked(const medDataIndex&)), d->preview, SLOT(addSeriesItem(const medDataIndex&)));

    connect(d->btOk, SIGNAL(clicked()), this, SLOT(onOkClicked()));
    connect(d->btCancel, SIGNAL(clicked()), this, SLOT(onCancelClicked()));

    // for the moment we just need patient and study
    for (int var = 2; var < d->proxy->columnCount(); ++var) {
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
    const int width = QApplication::desktop()->width();
    const int height = QApplication::desktop()->height();

    qDebug() << "WIDTH : " << width  << "HEIGHT : " << height ;

    return QSize(850, 700);
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
    qDebug() << "CANCEL";
    reject();
}

void medImageSelectionWidget::onOkClicked()
{
    qDebug() << "OK";
    accept();
}

QList<medDataIndex> medImageSelectionWidget::getSelectedIndexes()
{
    return d->selected->getContainedIndexes();
}
