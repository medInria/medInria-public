/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Sat Oct 10 00:15:26 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 133
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"
#include "medViewerAreaViewContainer.h"

#include <medGui/medLayoutChooser.h>
#include <medGui/medStyle.h>
#include <medGui/medToolBox.h>

#include <QtGui>

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

    QHash<int, medViewerAreaViewContainer *> view_containers;
};

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    QWidget *central = new QWidget(this);

    QVBoxLayout *c_layout_v = new QVBoxLayout(central);
    // c_layout_v->setContentsMargins(0, 0, 0, 0);
    // c_layout_v->setSpacing(0);

    QWidget *c_top = new QWidget(central);
    c_top->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    // QStackedWidget *stack = new QStackedWidget(central);

    d->view_container = new medViewerAreaViewContainer(central);
    d->view_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QComboBox *patientComboBox = new QComboBox(this);
    patientComboBox->addItem("Choose patient");
    patientComboBox->addItem("Patient A");
    patientComboBox->addItem("Patient B");
    patientComboBox->addItem("Patient C");
    connect(patientComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPatientIndexChanged(int)));

    QComboBox *studyComboBox = new QComboBox(this);
    studyComboBox->addItem("Choose study");
    studyComboBox->addItem("Study A");
    studyComboBox->addItem("Study B");
    studyComboBox->addItem("Study C");
    connect(studyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onStudyIndexChanged(int)));

    QComboBox *seriesComboBox = new QComboBox(this);
    seriesComboBox->addItem("Choose series");
    seriesComboBox->addItem("Series A");
    seriesComboBox->addItem("Series B");
    seriesComboBox->addItem("Series C");
    connect(seriesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSeriesIndexChanged(int)));
    
    QComboBox *imagesComboBox = new QComboBox(this);
    imagesComboBox->addItem("Choose image");
    imagesComboBox->addItem("Image A");
    imagesComboBox->addItem("Image B");
    imagesComboBox->addItem("Image C");
    connect(imagesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onImageIndexChanged(int)));

    QHBoxLayout *c_layout_h = new QHBoxLayout(c_top);
    c_layout_h->setContentsMargins(0, 0, 0, 0);
    c_layout_h->setSpacing(0);
    c_layout_h->addWidget(patientComboBox);
    c_layout_h->addWidget(studyComboBox);
    c_layout_h->addWidget(seriesComboBox);
    c_layout_h->addWidget(imagesComboBox);

    c_layout_v->addWidget(c_top);
    c_layout_v->addWidget(d->view_container);

    // Setting up toolboxes

    medLayoutChooser *layoutChooser = new medLayoutChooser;
    connect(layoutChooser, SIGNAL(selected(int,int)), this, SLOT(split(int,int)));

    QWidgetAction *layoutAction = new QWidgetAction(0);
    layoutAction->setDefaultWidget(layoutChooser);

    QMenu *layoutMenu = new QMenu;
    layoutMenu->addAction(layoutAction);

    QPushButton *undoLayoutButton = new QPushButton("Undo", this);
    QPushButton *  doLayoutButton = new QPushButton("    ", this);
    QPushButton *redoLayoutButton = new QPushButton("Redo", this);

    doLayoutButton->setMenu(layoutMenu);

    medToolBox *layoutToolBox = new medToolBox(this);
    layoutToolBox->setTitle("Layout");
    layoutToolBox->addWidget(undoLayoutButton);
    layoutToolBox->addWidget( doLayoutButton);
    layoutToolBox->addWidget(redoLayoutButton);
    
    // Setting up container

    d->toolbox_container = new medViewerAreaToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(layoutToolBox);
    d->toolbox_container->addToolBox(new medToolBox);
    d->toolbox_container->addToolBox(new medToolBox);

    // Setting up layout

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->toolbox_container);
    layout->addWidget(central);
}

medViewerArea::~medViewerArea(void)
{
    delete d->toolbox_container;
    delete d->view_container;
    delete d;

    d = NULL;
}

void medViewerArea::split(int rows, int cols)
{
    d->view_container->split(rows, cols);
}

void medViewerArea::onPatientIndexChanged(int index)
{
    if(!index)
        return;

    qDebug() << __func__ << index;
}

void medViewerArea::onStudyIndexChanged(int index)
{
    if(!index)
        return;

    qDebug() << __func__ << index;
}

void medViewerArea::onSeriesIndexChanged(int index)
{
    if(!index)
        return;

    qDebug() << __func__ << index;
}

void medViewerArea::onImageIndexChanged(int index)
{
    if(!index)
        return;

    qDebug() << __func__ << index;
}

medViewerAreaViewContainer *medViewerArea::current (void)
{
    return d->view_container->current();
}
