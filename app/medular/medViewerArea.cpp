/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 09:11:08 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 497
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"

#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractView.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkGlobal.h>

#include <medSql/medDatabaseController.h>

#include <medGui/medClutEditor.h>
#include <medGui/medLayoutChooser.h>
#include <medGui/medStyle.h>
#include <medGui/medToolBox.h>
#include <medGui/medToolBoxContainer.h>
#include <medGui/medViewContainer.h>

#include <QtGui>
#include <QtSql>

class medViewerAreaPrivate
{
public:
    medViewContainer *view_container;
    medToolBoxContainer *toolbox_container;

    QStackedWidget *stack;

    QComboBox *patientComboBox;
    QComboBox *studyComboBox;
    QComboBox *seriesComboBox;
    QComboBox *imagesComboBox;

    // Registration toolbox element

    QComboBox   *registrationWidgetCombo;
    QPushButton *registrationWidgetRegisterButton;
    QPushButton *registrationWidgetSaveButton;

    // view containers hash

    QHash<int, medViewContainer *> view_containers;

    // status elements

    QProgressBar *progress;
};

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    QWidget *main = new QWidget(this);

    QWidget *central = new QWidget(main);

    QVBoxLayout *c_layout_v = new QVBoxLayout(central);
    c_layout_v->setContentsMargins(0, 10, 0, 10);
    c_layout_v->setSpacing(10);

    QWidget *c_top = new QWidget(central);
    c_top->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    d->view_container = NULL;
    
    d->stack = new QStackedWidget(central);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->patientComboBox = new QComboBox(this);
    connect(d->patientComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPatientIndexChanged(int)));

    d->studyComboBox = new QComboBox(this);
    connect(d->studyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onStudyIndexChanged(int)));

    d->seriesComboBox = new QComboBox(this);
    connect(d->seriesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSeriesIndexChanged(int)));
    
    d->imagesComboBox = new QComboBox(this);
    connect(d->imagesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onImageIndexChanged(int)));

    QHBoxLayout *c_layout_h = new QHBoxLayout(c_top);
    c_layout_h->setContentsMargins(0, 0, 0, 0);
    c_layout_h->setSpacing(5);
    c_layout_h->addWidget(d->patientComboBox);
    c_layout_h->addWidget(d->studyComboBox);
    c_layout_h->addWidget(d->seriesComboBox);
    c_layout_h->addWidget(d->imagesComboBox);

    c_layout_v->addWidget(c_top);
    c_layout_v->addWidget(d->stack);

    // Setting up layout chooser

    medLayoutChooser *layoutChooser = new medLayoutChooser;
    connect(layoutChooser, SIGNAL(selected(int,int)), this, SLOT(split(int,int)));

    QWidgetAction *layoutAction = new QWidgetAction(0);
    layoutAction->setDefaultWidget(layoutChooser);

    QMenu *layoutMenu = new QMenu;
    layoutMenu->addAction(layoutAction);

    QPushButton *doLayoutButton = new QPushButton(this);
    doLayoutButton->setMenu(layoutMenu);

    medToolBox *layoutToolBox = new medToolBox(this);
    layoutToolBox->setTitle("Layout");
    layoutToolBox->setWidget(doLayoutButton);

    // Setting up lookup table editor

    medClutEditor *clutEditor = new medClutEditor;

    medToolBox *clutEditorToolBox = new medToolBox(this);
    clutEditorToolBox->setTitle("Color lookup table");
    clutEditorToolBox->setWidget(clutEditor);

    // Setting up container

    d->toolbox_container = new medToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(layoutToolBox);
    d->toolbox_container->addToolBox(clutEditorToolBox);

    // status elements

    d->progress = new QProgressBar(this);
    d->progress->setTextVisible(true);

    // Setting up layout

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->toolbox_container);
    layout->addWidget(central);

    // Setup from database
    this->setup();

    connect(medDatabaseController::instance(), SIGNAL(updated()), this, SLOT(setup()));
}

medViewerArea::~medViewerArea(void)
{
    delete d->toolbox_container;
    delete d->stack;
    delete d;

    d = NULL;
}

void medViewerArea::setup(QStatusBar *status)
{
    status->addPermanentWidget(d->progress);

    d->progress->show();
}

void medViewerArea::setdw(QStatusBar *status)
{
    status->removeWidget(d->progress);

    d->progress->hide();
}

void medViewerArea::setPatientIndex(int index)
{
    d->patientComboBox->setCurrentIndex(index);
}

void medViewerArea::setStudyIndex(int index)
{
    d->studyComboBox->setCurrentIndex(index);
}

void medViewerArea::setSeriesIndex(int index)
{
    d->seriesComboBox->setCurrentIndex(index);
}

void medViewerArea::setImageIndex(int index)
{
    d->imagesComboBox->setCurrentIndex(index);
}

void medViewerArea::setView(dtkAbstractView *view)
{
    d->view_container->current()->setView(view);
}

void medViewerArea::setup(void)
{
    d->patientComboBox->clear();
    d->studyComboBox->clear();
    d->seriesComboBox->clear();
    d->imagesComboBox->clear();
  
    d->patientComboBox->addItem("Choose patient");
    d->studyComboBox->addItem("Choose study");
    d->seriesComboBox->addItem("Choose series");
    d->imagesComboBox->addItem("Choose image");

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id FROM patient");
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    while(query.next())
        d->patientComboBox->addItem(query.value(0).toString(), query.value(1));
}

void medViewerArea::split(int rows, int cols)
{
    if (d->view_container)
        d->view_container->split(rows, cols);
}

void medViewerArea::onPatientIndexChanged(int index)
{
    if(index<1)
        return;

    // Setup view container

    medViewContainer *view_container;

    if(!d->view_containers.contains(index)) {
        view_container = new medViewContainer(this);
        d->view_containers.insert(index, view_container);
        d->stack->addWidget(view_container);
    } else {
        view_container = d->view_containers.value(index);
    }

    d->stack->setCurrentWidget(view_container);
    d->view_container = view_container;

    // Setup combos

    QVariant id = d->patientComboBox->itemData(index);

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id FROM study WHERE patient = :patient");
    query.bindValue(":patient", id.toInt());
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    d->studyComboBox->clear();
    d->studyComboBox->addItem("Choose study");
    d->seriesComboBox->clear();
    d->seriesComboBox->addItem("Choose series");
    d->imagesComboBox->clear();
    d->imagesComboBox->addItem("Choose image");

    while(query.next())
        d->studyComboBox->addItem(query.value(0).toString(), query.value(1));
}

void medViewerArea::onStudyIndexChanged(int index)
{
    if(index<1)
        return;

    QVariant id = d->studyComboBox->itemData(index);

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id FROM series WHERE study = :study");
    query.bindValue(":study", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    d->seriesComboBox->clear();
    d->seriesComboBox->addItem("Choose series");
    d->imagesComboBox->clear();
    d->imagesComboBox->addItem("Choose image");

    while(query.next())
        d->seriesComboBox->addItem(query.value(0).toString(), query.value(1));
}

void medViewerArea::onSeriesIndexChanged(int index)
{
    if(index<1)
        return;

    
    QSqlQuery query(*(medDatabaseController::instance()->database()));

    // Query and manage combos

    QVariant id = d->seriesComboBox->itemData(index);
    QStringList filenames;
    QString     filename;

    query.prepare("SELECT name, id, path, instance_path FROM image WHERE series = :series");
    query.bindValue(":series", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    d->imagesComboBox->clear();
    d->imagesComboBox->addItem("Choose image");

    while(query.next()) {
        d->imagesComboBox->addItem(query.value(0).toString(), query.value(1));
        filenames << query.value(2).toString();
	filename = query.value (3).toString();
    }    

    // Display data

    typedef dtkAbstractDataFactory::dtkAbstractDataTypeHandler dtkAbstractDataTypeHandler;
    
    dtkAbstractData *imData = NULL;
    
    QList<dtkAbstractDataTypeHandler> readers = dtkAbstractDataFactory::instance()->readers();

    for (int i=0; i<readers.size(); i++) {
        dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);

        if (dataReader->canRead(filename)) {
            dataReader->read(filename);
            imData = dataReader->data();
            delete dataReader;
            break;
        }
    }
    
    if (imData) {

        dtkAbstractView *view = dtkAbstractViewFactory::instance()->create ("v3dViewSwitcher");

        if (!view)
            return;

	if (!imData->hasMetaData ("PatientName"))
	  imData->addMetaData("PatientName", QStringList() << d->patientComboBox->currentText());
	if (!imData->hasMetaData ("StudyDescription"))
	  imData->addMetaData("StudyDescription", QStringList() << d->studyComboBox->currentText());
	if (!imData->hasMetaData ("SeriesDescription"))
	  imData->addMetaData("SeriesDescription", QStringList() << d->seriesComboBox->currentText());
        
        view->setData(imData);
        view->reset();

        this->setView(view);
    
	//delete imData;
    }

    emit seriesSelected(id.toInt());
}

void medViewerArea::onImageIndexChanged(int index)
{
    if(index<1)
        return;
}
