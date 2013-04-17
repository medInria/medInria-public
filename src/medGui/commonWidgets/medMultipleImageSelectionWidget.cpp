/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medMultipleImageSelectionWidget.h"

#include <QtGui>

#include <medDataManager.h>
#include <medDatabaseView.h>
#include <medDatabasePreview.h>
#include <medAbstractDbController.h>
#include <medDropSite.h>
#include <medThumbnailContainer.h>

class medMultipleImageSelectionWidgetPrivate
{
public:
    medDatabaseView *dbView;
    medThumbnailContainer *dbPreview;
    medThumbnailContainer *selectedImages;

    QPushButton* btOk;
    QPushButton* btCancel;
};

medMultipleImageSelectionWidget::medMultipleImageSelectionWidget(QList<medDataIndex> previouslySelectedIndexes, QWidget *parent) : d(new medMultipleImageSelectionWidgetPrivate)
{
    QList<medDataIndex> empty;
    d->dbPreview = new medThumbnailContainer(empty);
    d->dbPreview->setAllowDeleting(false);
    d->dbPreview->setAllowDragging(true);
    d->dbPreview->setAllowDropping(false);

    d->selectedImages = new medThumbnailContainer(previouslySelectedIndexes);
    d->selectedImages->setAllowDeleting(true);
    d->selectedImages->setAllowDragging(false);
    d->selectedImages->setAllowDropping(true);

    d->dbView = new medDatabaseView();

    d->btOk = new QPushButton(tr("OK"));
    d->btCancel = new QPushButton(tr("Cancel"));
    d->btOk->setDefault(true);

    // 3 main panels layout
    // db-view | db-preview
    // --------------------
    //   selected-images
    QVBoxLayout* mainLayout = new QVBoxLayout();

    QSplitter* splitterHorizontal = new QSplitter(Qt::Horizontal);
    QSplitter* splitterVertical = new QSplitter(Qt::Vertical);
    splitterHorizontal->setHandleWidth(1);
    splitterVertical->setHandleWidth(1);
    // we do not want to handle dynamic resizing
	splitterHorizontal->setOpaqueResize(false);
	splitterVertical->setOpaqueResize(false);

    splitterHorizontal->insertWidget(0, d->dbView);
    splitterHorizontal->insertWidget(1, d->dbPreview);

	splitterHorizontal->setStretchFactor(0, 1);
	splitterHorizontal->setStretchFactor(1, 3);

    splitterVertical->insertWidget(0, splitterHorizontal);
    splitterVertical->insertWidget(1, d->selectedImages);

    splitterVertical->setStretchFactor(0, 1);
    splitterVertical->setStretchFactor(1, 1);

    // buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setDirection(QBoxLayout::RightToLeft);

    int btWidth = 70;
    int btHeight = 45;
    d->btOk->setFixedSize(btWidth, btHeight);
    d->btCancel->setFixedSize(btWidth, btHeight);
    buttonsLayout->addWidget(d->btOk);
    buttonsLayout->addWidget(d->btCancel);

    mainLayout->addWidget(splitterVertical);
    mainLayout->addLayout(buttonsLayout);

    this->setLayout(mainLayout);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(d->dbView, SIGNAL(patientClicked(const medDataIndex&)), this, SLOT(onPatientSelected(const medDataIndex&)));
    connect(d->dbView, SIGNAL(seriesClicked(const medDataIndex&)), d->dbPreview, SLOT(addSeriesItem(const medDataIndex&)));

    connect(d->btOk, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
    connect(d->btCancel, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
}

medMultipleImageSelectionWidget::~medMultipleImageSelectionWidget(void)
{
    delete d;

    d = NULL;
}

QSize medMultipleImageSelectionWidget::sizeHint(void) const
{
    const int width = QApplication::desktop()->screen(0)->width();
    const int height = QApplication::desktop()->screen(0)->width();

    int widthHint = (int) (width * 0.6);
    int heightHint = (int) (height * 0.6);

    return QSize(widthHint, heightHint);
}

void medMultipleImageSelectionWidget::setModel(QAbstractItemModel* model)
{
    d->dbView->setModel(model);

    // for the moment we just need patient and study
    for (int var = 2; var < model->columnCount(); ++var) {
        d->dbView->hideColumn(var);
    }
}

void medMultipleImageSelectionWidget::onPatientSelected(const medDataIndex& patientId)
{
    // when a patient is selected on the dbView we add the series
    // belonging to that patient to the dbPreview panel
    d->dbPreview->reset();

    medAbstractDbController* db =  medDataManager::instance()->controllerForDataSource(patientId.dataSourceId());
    if (db)
    {
        QList<medDataIndex> studies = db->studies(patientId);
        for (QList<medDataIndex>::const_iterator studyIt( studies.begin() ); studyIt != studies.end(); ++studyIt )
        {
            QList<medDataIndex> series = db->series(*studyIt);
            for (QList<medDataIndex>::const_iterator seriesIt( series.begin() ); seriesIt != series.end(); ++seriesIt )
            {
                d->dbPreview->addSeriesItem(*seriesIt);
            }
        }
    }
}

void medMultipleImageSelectionWidget::onCancelButtonClicked()
{
//    qDebug() << "Cancel button clicked";
    reject();
}

void medMultipleImageSelectionWidget::onOkButtonClicked()
{
//    qDebug() << "OK button clicked";
    accept();
}

QList<medDataIndex> medMultipleImageSelectionWidget::getSelectedIndexes()
{
    return d->selectedImages->getContainedIndexes();
}
