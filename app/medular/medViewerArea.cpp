/* medViewerArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 18 12:43:06 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Sep 22 15:59:36 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 50
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medViewerArea.h"

#include <medGui/medLayoutChooser.h>

#include <QtGui>

///////////////////////////////////////////////////////////////////
// medViewerAreaToolBoxHeader
///////////////////////////////////////////////////////////////////

class medViewerAreaToolBoxHeader : public QWidget
{
public:
     medViewerAreaToolBoxHeader(QWidget *parent = 0);
    ~medViewerAreaToolBoxHeader(void);

    void setTitle(const QString& title);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QLabel *label;
    QLabel *button;
};

medViewerAreaToolBoxHeader::medViewerAreaToolBoxHeader(QWidget *parent) : QWidget(parent)
{
    this->label = new QLabel(this);
    this->label->setText("Untitled");
    this->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    this->button = new QLabel(this);
    this->button->setText("?");
    this->button->setFixedWidth(16);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(0);
    layout->addWidget(this->label);
    layout->addWidget(this->button);

    this->setStyleSheet(
     "color: white;"
     "font-size: 9px;"
    );

    this->setFixedHeight(20);
}

medViewerAreaToolBoxHeader::~medViewerAreaToolBoxHeader(void)
{
    delete this->label;
    delete this->button;
}

void medViewerAreaToolBoxHeader::setTitle(const QString& title)
{
    this->label->setText(title);
}

void medViewerAreaToolBoxHeader::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setPen(Qt::darkGray);
    p.drawLine(event->rect().bottomLeft(), event->rect().bottomRight());
}

///////////////////////////////////////////////////////////////////
// medViewerAreaToolBoxContent
///////////////////////////////////////////////////////////////////

class medViewerAreaToolBoxContent : public QWidget
{
public:
     medViewerAreaToolBoxContent(QWidget *parent = 0);
    ~medViewerAreaToolBoxContent(void);

    void addWidget(QWidget *widget);

private:
    int row;
    int col;
    QGridLayout *layout;
};

medViewerAreaToolBoxContent::medViewerAreaToolBoxContent(QWidget *parent) : QWidget(parent)
{    
    this->layout = new QGridLayout(this);

    this->row = 0;
    this->col = 0;
}

medViewerAreaToolBoxContent::~medViewerAreaToolBoxContent(void)
{

}

void medViewerAreaToolBoxContent::addWidget(QWidget *widget)
{
    this->layout->addWidget(widget, row, col);

    if (col > 3) {
        row++;
        col = 0;
    } else {
        col++;
    }
}

///////////////////////////////////////////////////////////////////
// medViewerAreaToolBox
///////////////////////////////////////////////////////////////////

class medViewerAreaToolBox : public QWidget
{
public:
     medViewerAreaToolBox(QWidget *parent = 0);
    ~medViewerAreaToolBox(void);

    void addWidget(QWidget *widget);

    void setTitle(const QString& title);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medViewerAreaToolBoxHeader *header;
    medViewerAreaToolBoxContent *content;
};

medViewerAreaToolBox::medViewerAreaToolBox(QWidget *parent) : QWidget(parent)
{
    this->header = new medViewerAreaToolBoxHeader(this);
    this->content = new medViewerAreaToolBoxContent(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(this->header);
    layout->addWidget(this->content);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

medViewerAreaToolBox::~medViewerAreaToolBox(void)
{
    delete this->header;
    delete this->content;
}

void medViewerAreaToolBox::addWidget(QWidget *widget)
{
    this->content->addWidget(widget);
}

void medViewerAreaToolBox::setTitle(const QString &title)
{
    this->header->setTitle(title);
}

void medViewerAreaToolBox::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setPen(Qt::darkGray);
    p.drawRect(event->rect().adjusted(0, 0, -1, -1));
}

///////////////////////////////////////////////////////////////////
// medViewerAreaToolBoxContainer
///////////////////////////////////////////////////////////////////

class medViewerAreaToolBoxContainer : public QScrollArea
{
public:
     medViewerAreaToolBoxContainer(QWidget *parent = 0);
    ~medViewerAreaToolBoxContainer(void);

    void addToolBox(medViewerAreaToolBox *toolBox);
    void removeToolBox(medViewerAreaToolBox *toolBox);

private:
    QWidget *container;
    QVBoxLayout *layout;
};

medViewerAreaToolBoxContainer::medViewerAreaToolBoxContainer(QWidget *parent) : QScrollArea(parent)
{
    this->container = new QWidget(this);
    this->layout = new QVBoxLayout(this->container);
    this->layout->addStretch(1);

    this->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    this->setWidget(this->container);
    this->setWidgetResizable(true);
}

medViewerAreaToolBoxContainer::~medViewerAreaToolBoxContainer(void)
{

}

void medViewerAreaToolBoxContainer::addToolBox(medViewerAreaToolBox *toolBox)
{
    this->layout->insertWidget(this->layout->count()-1, toolBox, 0, Qt::AlignTop);
}

void medViewerAreaToolBoxContainer::removeToolBox(medViewerAreaToolBox *toolBox)
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
            current->m_layout->addWidget(new medViewerAreaViewContainer(this), i, j);
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

static void paintLayout(QPainter *painter, QLayoutItem *item)
{
    if (QLayout *layout = item->layout())
        for (int i = 0; i < layout->count(); ++i)
            paintLayout(painter, layout->itemAt(i));

    medViewerAreaViewContainer *view_container = dynamic_cast<medViewerAreaViewContainer *>(item->widget());

    if(!view_container)
        return;

    view_container->hasFocus() ? painter->setPen(Qt::red) : painter->setPen(Qt::gray);

    painter->drawRect(item->geometry().adjusted(0, 0, -1, -1));
}

void medViewerAreaViewContainer::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (layout())
        paintLayout(&painter, layout());
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
};

medViewerArea::medViewerArea(QWidget *parent) : QWidget(parent), d(new medViewerAreaPrivate)
{
    QWidget *central = new QWidget(this);

    QVBoxLayout *c_layout_v = new QVBoxLayout(central);
    c_layout_v->setContentsMargins(0, 0, 0, 0);
    c_layout_v->setSpacing(0);

    QWidget *c_top = new QWidget(central);
    c_top->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    d->view_container = new medViewerAreaViewContainer(central);
    d->view_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QComboBox *patientComboBox = new QComboBox(this);
    patientComboBox->addItem("Patient A");
    patientComboBox->addItem("Patient B");
    patientComboBox->addItem("Patient C");
    connect(patientComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPatientIndexChanged(int)));

    QComboBox *studyComboBox = new QComboBox(this);
    studyComboBox->addItem("Study A");
    studyComboBox->addItem("Study B");
    studyComboBox->addItem("Study C");
    connect(studyComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onStudyIndexChanged(int)));

    QComboBox *seriesComboBox = new QComboBox(this);
    seriesComboBox->addItem("Series A");
    seriesComboBox->addItem("Series B");
    seriesComboBox->addItem("Series C");
    connect(seriesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSeriesIndexChanged(int)));
    
    QComboBox *imagesComboBox = new QComboBox(this);
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

    medViewerAreaToolBox *layoutToolBox = new medViewerAreaToolBox(this);
    layoutToolBox->setTitle("Layout");
    layoutToolBox->addWidget(undoLayoutButton);
    layoutToolBox->addWidget( doLayoutButton);
    layoutToolBox->addWidget(redoLayoutButton);
    
    // Setting up container

    d->toolbox_container = new medViewerAreaToolBoxContainer(this);
    d->toolbox_container->setFixedWidth(300);
    d->toolbox_container->setStyleSheet(
        "border-right-width: 1px;"
        "border-right-style: solid;"
        "border-right-color: darkGray;");
    
    d->toolbox_container->addToolBox(layoutToolBox);
    d->toolbox_container->addToolBox(new medViewerAreaToolBox);
    d->toolbox_container->addToolBox(new medViewerAreaToolBox);

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
    qDebug() << __func__ << index;
}

void medViewerArea::onStudyIndexChanged(int index)
{
    qDebug() << __func__ << index;
}

void medViewerArea::onSeriesIndexChanged(int index)
{
    qDebug() << __func__ << index;
}

void medViewerArea::onImageIndexChanged(int index)
{
    qDebug() << __func__ << index;
}
