/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct 20 09:59:54 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 418
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
#include <medGui/medStatusPanel.h>
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
// medViewerAreaViewContainer
///////////////////////////////////////////////////////////////////

class medViewerAreaViewContainer : public QWidget
{
public:
     medViewerAreaViewContainer(QWidget *parent = 0);
    ~medViewerAreaViewContainer(void);

    medViewerAreaViewContainer *current(void);

    void split(int rows, int cols);

    void setView(dtkAbstractView *view);

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QGridLayout *m_layout;

    static medViewerAreaViewContainer *s_current;
};

medViewerAreaViewContainer::medViewerAreaViewContainer(QWidget *parent) : QWidget(parent)
{
    m_layout = new QGridLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);

    s_current = this;

    this->setFocusPolicy(Qt::StrongFocus);
}

medViewerAreaViewContainer::~medViewerAreaViewContainer(void)
{
    delete m_layout;
}

medViewerAreaViewContainer *medViewerAreaViewContainer::current(void)
{
    return s_current;
}

void medViewerAreaViewContainer::split(int rows, int cols)
{
    medViewerAreaViewContainer *current = this->current();

    if (!current)
        return;

    if (current->m_layout->count())
        return;

    for(int i = 0 ; i < rows ; i++)
        for(int j = 0 ; j < cols ; j++)
            current->m_layout->addWidget(new medViewerAreaViewContainer(current), i, j);

    s_current = 0;
}

void medViewerAreaViewContainer::setView(dtkAbstractView *view)
{
    medViewerAreaViewContainer *current = this->current();

    if (!current)
        return;

    if (current->m_layout->count())
        return;

    if(QWidget *widget = view->widget()) {
        widget->setParent(this);
        widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        current->m_layout->setContentsMargins(1, 1, 1, 1);
        current->m_layout->addWidget(widget, 0, 0);
    }
}

void medViewerAreaViewContainer::focusInEvent(QFocusEvent *event)
{
    s_current = this;

    QWidget::focusInEvent(event);
}

void medViewerAreaViewContainer::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
}

void medViewerAreaViewContainer::paintEvent(QPaintEvent *event)
{
    if(this->layout()->count() > 1)
        return;

    QWidget::paintEvent(event);

    QPainter painter;
    painter.begin(this);
    if (s_current == this)
        painter.setPen(Qt::red);
    else
        painter.setPen(Qt::darkGray);
    painter.setBrush(QColor(0x38, 0x38, 0x38));
    painter.drawRect(this->rect().adjusted(0, 0, -1, -1));
    painter.end();
}

medViewerAreaViewContainer *medViewerAreaViewContainer::s_current = NULL;

///////////////////////////////////////////////////////////////////
// medViewerArea
///////////////////////////////////////////////////////////////////

class medViewerAreaPrivate
{
public:
    medViewerAreaViewContainer *view_container;
    medViewerAreaToolBoxContainer *toolbox_container;
    medStatusPanel *status;

    QStackedWidget *stack;

    QComboBox *patientComboBox;
    QComboBox *studyComboBox;
    QComboBox *seriesComboBox;
    QComboBox *imagesComboBox;

    QHash<int, medViewerAreaViewContainer *> view_containers;
};

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    QWidget *main = new QWidget(this);

    QWidget *central = new QWidget(main);

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

    // Setting up visualization tools

    QPushButton *button0 = new QPushButton;
    QPushButton *button1 = new QPushButton;
    QPushButton *button2 = new QPushButton;
    QPushButton *button3 = new QPushButton;
    QPushButton *button4 = new QPushButton;
    QPushButton *button5 = new QPushButton;
    QPushButton *button6 = new QPushButton;
    QPushButton *button7 = new QPushButton;
    QPushButton *button8 = new QPushButton;
    QPushButton *button9 = new QPushButton;

    medToolBox *visualizationToolBox = new medToolBox(this);
    visualizationToolBox->setTitle("Visualization");
    visualizationToolBox->addWidget(button0);
    visualizationToolBox->addWidget(button1);
    visualizationToolBox->addWidget(button2);
    visualizationToolBox->addWidget(button3);
    visualizationToolBox->addWidget(button4);
    visualizationToolBox->addWidget(button5);
    visualizationToolBox->addWidget(button6);
    visualizationToolBox->addWidget(button7);
    visualizationToolBox->addWidget(button8);
    visualizationToolBox->addWidget(button9);

    // Setting up lookup table editor

    medClutEditor *clutEditor = new medClutEditor;

    medToolBox *clutEditorToolBox = new medToolBox(this);
    clutEditorToolBox->setTitle("Color lookup table");
    clutEditorToolBox->addWidget(clutEditor);

    // Setting up container

    d->toolbox_container = new medViewerAreaToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->addToolBox(layoutToolBox);
    d->toolbox_container->addToolBox(visualizationToolBox);
    d->toolbox_container->addToolBox(clutEditorToolBox);

    // Setting up panel

    d->status = new medStatusPanel(this);

    // Setting up layout

    QHBoxLayout *central_layout = new QHBoxLayout(main);
    central_layout->setContentsMargins(10, 10, 10, 10);
    central_layout->setSpacing(10);
    central_layout->addWidget(d->toolbox_container);
    central_layout->addWidget(central);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(main);
    layout->addWidget(d->status);

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

    // Query and manage combos

    QVariant id = d->seriesComboBox->itemData(index);
    QStringList filenames;

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    query.prepare("SELECT name, id, path FROM image WHERE series = :series");
    query.bindValue(":series", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    d->imagesComboBox->clear();
    d->imagesComboBox->addItem("Choose image");

    while(query.next()) {
        d->imagesComboBox->addItem(query.value(0).toString(), query.value(1));
        filenames << query.value(2).toString();
    }

    // Display data

    typedef dtkAbstractDataFactory::dtkAbstractDataTypeHandler dtkAbstractDataTypeHandler;
    
    dtkAbstractData *imData = NULL;
    
    QList<dtkAbstractDataTypeHandler> readers = dtkAbstractDataFactory::instance()->readers();

    for (int i=0; i<readers.size(); i++) {
        dtkAbstractDataReader* dataReader = dtkAbstractDataFactory::instance()->reader(readers[i].first, readers[i].second);

        if (dataReader->canRead(filenames)) {
            dataReader->read(filenames);
            imData = dataReader->data();
            delete dataReader;
            break;
        }
    }
    
    if (imData) {
        dtkAbstractView *view = dtkAbstractViewFactory::instance()->create ("v3dView2D");

        if (!view)
            return;
        
        view->setData(imData);
        view->reset();

        this->setView(view);
    }

    emit seriesSelected(id.toInt());
}

void medViewerArea::onImageIndexChanged(int index)
{
    if(index<1)
        return;
}
