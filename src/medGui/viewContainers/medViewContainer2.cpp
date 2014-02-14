/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainer2.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <medAbstractView.h>
#include <medBoolParameter.h>
#include <medDataIndex.h>
#include <medAbstractData.h>
#include <medDataManager.h>
#include <medImageViewFactory.h>
#include <medLayeredViewFactory.h>
#include <medViewManager.h>

class medViewContainer2Private
{
public:
    medAbstractView* view;
    bool selected;
    bool maximised;
    QVBoxLayout* mainLayout;
    QHBoxLayout* toolBarLayout;
    QPushButton* vSplittButton;
    QPushButton* hSplittButton;
    QPushButton* rmSplittButton;

    medBoolParameter* maximisedParameter;

    ~medViewContainer2Private()
    {
        if(view)
            delete view;
    }
};


medViewContainer2::medViewContainer2(QWidget *parent): d(new medViewContainer2Private)
{
    QPushButton* closeButton = new QPushButton(this);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    d->vSplittButton = new QPushButton(this);
    connect(d->vSplittButton, SIGNAL(clicked()), this, SIGNAL(vSplitRequest()));
    d->hSplittButton = new QPushButton(this);
    connect(d->hSplittButton, SIGNAL(clicked()), this, SIGNAL(hSplitRequest()));


    d->maximisedParameter = new medBoolParameter("maximied view", this);
    d->maximised = true;
    connect(d->maximisedParameter, SIGNAL(valueChanged(bool)), this, SLOT(setMaximised(bool)));
    connect(this, SIGNAL(maximised(bool)), d->maximisedParameter, SLOT(setValue(bool)));
    d->maximisedParameter->setValue(false);
    d->maximisedParameter->hide();

    QWidget* toolBar = new QWidget(this);
    d->toolBarLayout = new QHBoxLayout(toolBar);
    d->toolBarLayout->addWidget(d->maximisedParameter->getPushButton(), 0, Qt::AlignRight);
    d->toolBarLayout->addWidget(d->vSplittButton, 0, Qt::AlignRight);
    d->toolBarLayout->addWidget(d->hSplittButton, 0, Qt::AlignRight);
    d->toolBarLayout->addWidget(closeButton, 0, Qt::AlignRight);

    d->mainLayout = new QVBoxLayout(this);
    d->mainLayout->setContentsMargins(0, 0, 0, 0);
    d->mainLayout->addWidget(toolBar);

    this->setSelected(true);
}

medViewContainer2::~medViewContainer2()
{
    delete d;
}

medAbstractView* medViewContainer2::view() const
{
    return d->view;
}

void medViewContainer2::setView(medAbstractView *view)
{
    if(d->view)
        delete d->view;

    d->view = view;
    connect(d->view, SIGNAL(destroyed()), this, SLOT(removeInterneView()));

    d->toolBarLayout->insertWidget(0, d->view->viewWidget());
    d->maximisedParameter->show();
    d->mainLayout->addWidget(d->view->viewWidget());
}

bool medViewContainer2::isSelected() const
{
    return d->selected;
}

void medViewContainer2::setSelected(bool selec)
{
    d->selected = selec;

    if(d->selected)
    {
        this->setProperty("selected", QVariant("true"));
//        medViewManager::instance()->addToSelection(d->view);
        emit selected();
    }
    else
    {
        this->setProperty("selected", QVariant("false"));
    }

    this->recomputeStyleSheet();
}

void medViewContainer2::setUnSelected(bool unSelected)
{
    this->setSelected(!unSelected);
}

void medViewContainer2::setMaximised(bool maxi)
{
    if(d->maximised == maxi)
        return;

    if(maxi)
    {
        d->vSplittButton->hide();
        d->hSplittButton->hide();
        if(d->rmSplittButton)
            d->rmSplittButton->hide();
    }
    else
    {
        d->vSplittButton->show();
        d->hSplittButton->show();
        if(d->rmSplittButton)
            d->rmSplittButton->show();
    }

    emit maximised(maxi);
}

void medViewContainer2::removeInterneView()
{
    d->view = NULL;
    d->maximisedParameter->hide();
}

void medViewContainer2::focusInEvent(QFocusEvent *event)
{
    this->setSelected(true);
    QFrame::focusInEvent(event);
}

void medViewContainer2::recomputeStyleSheet()
{
    this->style()->unpolish(this);
    this->style()->polish(this);
}

void medViewContainer2::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void medViewContainer2::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void medViewContainer2::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void medViewContainer2::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    medDataIndex index = medDataIndex::readMimeData(mimeData);
    if(index.isValidForSeries())
        this->addData(medDataManager::instance()->data(index));

    event->acceptProposedAction();
}

void medViewContainer2::addData(medAbstractData *data)
{
    if(!data)
        return;
    //TODO find from data(factory?) which view have to be created - RDE
    if(!d->view)
    {
        d->view = medImageViewFactory::instance()->createView("medVtkView", this);
        if(!d->view)
        {
            qWarning() << "medViewContainer: Unable to create a medVtkView";
            return;
        }

        this->setView(d->view);
    }

    //TODO bring a way to kow how to add the data to the view.
    //     assuming by now that we always have layered view.
    //     this can be did by having a method that return the base abstract class(categorie)
    //     of the view. - RDE
    medAbstractLayeredView* view = dynamic_cast<medAbstractLayeredView*>(d->view);
    view->addLayer(data);
}


void medViewContainer2::addRmSplittButton()
{
    if(d->rmSplittButton)
        return;

    d->rmSplittButton = new QPushButton(this);
    connect(d->rmSplittButton, SIGNAL(clicked()), this, SIGNAL(rmSplitRequest()));
    d->toolBarLayout->insertWidget(d->toolBarLayout->count() - 2, d->rmSplittButton, 0, Qt::AlignRight);
}
