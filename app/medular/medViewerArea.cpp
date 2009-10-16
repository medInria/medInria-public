/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct 16 13:35:51 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 282
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"
#include "medViewerAreaViewContainer.h"

#include <dtkCore/dtkGlobal.h>

#include <medSql/medDatabaseController.h>

#include <medGui/medClutEditor.h>
#include <medGui/medLayoutChooser.h>
#include <medGui/medStyle.h>
#include <medGui/medToolBox.h>

#include <QtGui>
#include <QtSql>

///////////////////////////////////////////////////////////////////
// medViewerAreaToolBoxContainer
///////////////////////////////////////////////////////////////////

class medViewerAreaToolBoxContainer : public QScrollArea
{
public:
     medViewerAreaToolBoxContainer(QWidget *parent = 0);
    ~medViewerAreaToolBoxContainer(void);

    void addToolBox(medToolBox *toolBox);
    void removeToolBox(medToolBox *toolBox);

private:
    QWidget *container;
    QVBoxLayout *layout;
};

medViewerAreaToolBoxContainer::medViewerAreaToolBoxContainer(QWidget *parent) : QScrollArea(parent)
{
    this->container = new QWidget(this);
    this->layout = new QVBoxLayout(this->container);
    this->layout->setContentsMargins(0, 0, 0, 0);
    this->layout->addStretch(1);

    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setWidget(this->container);
    this->setWidgetResizable(true);
}

medViewerAreaToolBoxContainer::~medViewerAreaToolBoxContainer(void)
{

}

void medViewerAreaToolBoxContainer::addToolBox(medToolBox *toolBox)
{
    this->layout->insertWidget(this->layout->count()-1, toolBox, 0, Qt::AlignTop);
}

void medViewerAreaToolBoxContainer::removeToolBox(medToolBox *toolBox)
{
    this->layout->removeWidget(toolBox);
}

///////////////////////////////////////////////////////////////////
// medViewerArea
///////////////////////////////////////////////////////////////////

class medViewerAreaPrivate
{
public:
    medViewerAreaViewContainer *view_container;
    medViewerAreaToolBoxContainer *toolbox_container;

    QStackedWidget *stack;

    QComboBox *patientComboBox;
    QComboBox *studyComboBox;
    QComboBox *seriesComboBox;
    QComboBox *imagesComboBox;

    QHash<int, medViewerAreaViewContainer *> view_containers;
};

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    QWidget *central = new QWidget(this);

    QVBoxLayout *c_layout_v = new QVBoxLayout(central);
    c_layout_v->setContentsMargins(0, 0, 0, 0);
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
    c_layout_h->setSpacing(0);
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
    layoutToolBox->addWidget(doLayoutButton);

    // Setting up lookup table editor

    medClutEditor *clutEditor = new medClutEditor;

    medToolBox *clutEditorToolBox = new medToolBox(this);
    clutEditorToolBox->setTitle("Color lookup table");
    clutEditorToolBox->addWidget(clutEditor);

    // Setting up container

    d->toolbox_container = new medViewerAreaToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(layoutToolBox);
    d->toolbox_container->addToolBox(clutEditorToolBox);

    // Setting up layout

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);
    layout->addWidget(d->toolbox_container);
    layout->addWidget(central);

    // Setup from database
    this->setup();
}

medViewerArea::~medViewerArea(void)
{
    delete d->toolbox_container;
    delete d->stack;
    delete d;

    d = NULL;
}

void medViewerArea::setup(void)
{
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
    if(!index)
        return;

    // Setup view container

    medViewerAreaViewContainer *view_container;

    if(!d->view_containers.contains(index)) {
        view_container = new medViewerAreaViewContainer(this);
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
    if(!index)
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
    if(!index)
        return;

    QVariant id = d->seriesComboBox->itemData(index);

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id FROM image WHERE series = :series");
    query.bindValue(":series", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    d->imagesComboBox->clear();
    d->imagesComboBox->addItem("Choose image");

    while(query.next())
        d->imagesComboBox->addItem(query.value(0).toString(), query.value(1));
}

void medViewerArea::onImageIndexChanged(int index)
{
    if(!index)
        return;
}

medViewerAreaViewContainer *medViewerArea::current (void)
{
    return d->view_container->current();
}
