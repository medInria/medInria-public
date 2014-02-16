/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewSplitableContainer.h>

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
    QFrame *emptyView;
    bool selected;
    bool maximised;
    QGridLayout* mainLayout;
    QHBoxLayout* toolBarLayout;
    QPushButton* vSplittButton;
    QPushButton* hSplittButton;

    QLabel *northDragLabel;
    QLabel *eastDragLabel;
    QLabel *southDragLabel;
    QLabel *westDragLabel;
    QLabel *midDragLabel;

    int receiverQuarterWidth;
    int receiverQuarterHeight;

    medBoolParameter* maximisedParameter;

    ~medViewContainer2Private()
    {
        if(view)
            delete view;
    }
};


medViewSplitableContainer::medViewSplitableContainer(QWidget *parent): QWidget(parent),
    d(new medViewContainer2Private)
{
    this->setObjectName("container");
    d->view = NULL;
    d->northDragLabel = NULL;
    d->eastDragLabel = NULL;
    d->westDragLabel = NULL;
    d->southDragLabel = NULL;
    d->midDragLabel = NULL;

    d->emptyView = new QFrame(this);
    d->emptyView->setObjectName("emptyView");;
    d->emptyView->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);

    QPushButton* closeButton = new QPushButton(this);
    closeButton->setIcon(QIcon(":/medGui/pixmaps/closebutton.png"));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(selfDestroy()));
    closeButton->setMaximumHeight(18);

    d->vSplittButton = new QPushButton(this);
    d->vSplittButton->setMaximumHeight(18);
    d->vSplittButton->setIcon(QIcon(":/medGui/pixmaps/splitbutton_vertical.png"));
    connect(d->vSplittButton, SIGNAL(clicked()), this, SIGNAL(vSplitRequest()));
    d->hSplittButton = new QPushButton(this);
    d->hSplittButton->setMaximumHeight(18);
    d->hSplittButton->setIcon(QIcon(":/medGui/pixmaps/splitbutton_horizontal.png"));
    connect(d->hSplittButton, SIGNAL(clicked()), this, SIGNAL(hSplitRequest()));


    d->maximisedParameter = new medBoolParameter("maximied view", this);
    d->maximisedParameter->getPushButton()->setMaximumHeight(18);
    QIcon maximisedIcon(":/icons/maximize.svg");
    maximisedIcon.addFile(":/icons/un_maximize.svg",
                        QSize(16,16),
                        QIcon::Normal,
                        QIcon::On);
    d->maximisedParameter->setIcon(maximisedIcon);
    d->maximised = true;
    connect(d->maximisedParameter, SIGNAL(valueChanged(bool)), this, SLOT(setMaximised(bool)));
    connect(this, SIGNAL(maximised(bool)), d->maximisedParameter, SLOT(setValue(bool)));
    d->maximisedParameter->setValue(false);
    d->maximisedParameter->hide();

    QWidget* toolBar = new QWidget(this);
    toolBar->setObjectName("containerToolBar");
    toolBar->setMaximumHeight(35);
    toolBar->setContentsMargins(0,0,0,0);
    d->toolBarLayout = new QHBoxLayout(toolBar);
    d->toolBarLayout->setSpacing(0);
    d->toolBarLayout->addWidget(d->maximisedParameter->getPushButton(), 1, Qt::AlignRight);
    d->toolBarLayout->addWidget(d->vSplittButton, 1, Qt::AlignRight);
    d->toolBarLayout->addWidget(d->hSplittButton, 1, Qt::AlignRight);
    d->toolBarLayout->addWidget(closeButton, 1, Qt::AlignRight);

    d->mainLayout = new QGridLayout(this);
    d->mainLayout->setContentsMargins(0, 0, 0, 0);
    d->mainLayout->addWidget(toolBar, 0, 0);
    d->mainLayout->addWidget(d->emptyView, 1, 0);

    this->setAcceptDrops(true);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setMouseTracking(true);
    this->setSelected(true);
}

medViewSplitableContainer::~medViewSplitableContainer()
{
    //trick to 'inform' a parented splitter
    //"you're not my dad anymore!"
    this->setParent(NULL);
    delete d;
}

medAbstractView* medViewSplitableContainer::view() const
{
    return d->view;
}

void medViewSplitableContainer::setView(medAbstractView *view)
{
    if(d->view)
        delete d->view;
    else
        d->mainLayout->removeWidget(d->emptyView);

    d->view = view;
    connect(d->view, SIGNAL(destroyed()), this, SLOT(removeInterneView()));

    d->toolBarLayout->insertWidget(0, d->view->toolBar());
    d->maximisedParameter->show();
    d->mainLayout->addWidget(d->view->viewWidget(), 1, 0, 1, 1);
}

bool medViewSplitableContainer::isSelected() const
{
    return d->selected;
}

void medViewSplitableContainer::setSelected(bool selec)
{
    d->selected = selec;
    if(d->selected)
    {
        this->setProperty("selected", QVariant(true));
//        medViewManager::instance()->addToSelection(d->view);
        emit selected();
    }
    else
    {
        this->setProperty("selected", QVariant(false));
    }
    this->recomputeStyleSheet();
}

void medViewSplitableContainer::setUnSelected(bool unSelected)
{
    this->setSelected(!unSelected);
}

void medViewSplitableContainer::setMaximised(bool maxi)
{
    if(d->maximised == maxi)
        return;

    if(maxi)
    {
        d->vSplittButton->hide();
        d->hSplittButton->hide();
    }
    else
    {
        d->vSplittButton->show();
        d->hSplittButton->show();
    }

    emit maximised(maxi);
}

void medViewSplitableContainer::removeInterneView()
{
    d->view = NULL;
    d->maximisedParameter->hide();
    d->mainLayout->addWidget(d->emptyView);
}

void medViewSplitableContainer::focusInEvent(QFocusEvent *event)
{
    this->setSelected(true);
    QWidget::focusInEvent(event);
}

void medViewSplitableContainer::recomputeStyleSheet()
{
    this->style()->unpolish(this);
    this->style()->polish(this);
}

void medViewSplitableContainer::dragEnterEvent(QDragEnterEvent *event)
{
    this->createDragLabels();
    event->acceptProposedAction();
}

void medViewSplitableContainer::dragMoveEvent(QDragMoveEvent *event)
{
    int x(event->pos().x()), y(event->pos().y());
    int rqw(d->receiverQuarterWidth), rqh(d->receiverQuarterHeight);

    if((x > rqw && x < rqw * 3) && (y < rqh))
    {
        d->northDragLabel->show();
        d->eastDragLabel->hide();
        d->southDragLabel->hide();
        d->westDragLabel->hide();
        d->midDragLabel->hide();
    }
    else if((x > rqw * 3) && (y > rqh && y < rqh * 3))
    {
        d->northDragLabel->hide();
        d->eastDragLabel->show();
        d->southDragLabel->hide();
        d->westDragLabel->hide();
        d->midDragLabel->hide();
    }
    else if((x > rqw && x < rqw * 3) && (y > rqh * 3))
    {
        d->northDragLabel->hide();
        d->eastDragLabel->hide();
        d->southDragLabel->show();
        d->westDragLabel->hide();
        d->midDragLabel->hide();
    }
    else if((x < rqw) && (y > rqh && y < rqh * 3))
    {
        d->northDragLabel->hide();
        d->eastDragLabel->hide();
        d->southDragLabel->hide();
        d->westDragLabel->show();
        d->midDragLabel->hide();
    }
    else if((x > rqw && x < rqw * 3) && (y > rqh && y < rqh * 3))
    {
        d->northDragLabel->hide();
        d->eastDragLabel->hide();
        d->southDragLabel->hide();
        d->westDragLabel->hide();
        d->midDragLabel->show();
    }

    event->acceptProposedAction();
}

void medViewSplitableContainer::dragLeaveEvent(QDragLeaveEvent *event)
{
    this->destroyDragLabels();
    event->accept();
}

void medViewSplitableContainer::dropEvent(QDropEvent *event)
{
    this->destroyDragLabels();

    const QMimeData *mimeData = event->mimeData();
    medDataIndex index = medDataIndex::readMimeData(mimeData);
    if(!index.isValidForSeries())
        return;

    int x(event->pos().x()), y(event->pos().y());
    int rqw(d->receiverQuarterWidth), rqh(d->receiverQuarterHeight);

    if((x > rqw && x < rqw * 3) && (y < rqh))
        emit splitRequest(index, Qt::AlignTop);
    else if((x > rqw * 3) && (y > rqh && y < rqh * 3))
        emit splitRequest(index, Qt::AlignRight);
    else if((x > rqw && x < rqw * 3) && (y > rqh * 3))
        emit splitRequest(index, Qt::AlignBottom);
    else if((x < rqw) && (y > rqh && y < rqh * 3))
        emit splitRequest(index, Qt::AlignLeft);
    else if((x > rqw && x < rqw * 3) && (y > rqh && y < rqh * 3))
        this->addData(medDataManager::instance()->data(index));

    event->acceptProposedAction();
}

void medViewSplitableContainer::addData(medAbstractData *data)
{
    if(!data)
        return;

    if(!d->view)
    {
        //TODO find from data(factory?) which view have to be created - RDE
        medAbstractLayeredView* view;
        view = medImageViewFactory::instance()->createView("medVtkView", this);
        if(!view)
        {
            qWarning() << "medViewContainer: Unable to create a medVtkView";
            return;
        }
        this->setView(view);
    }
    //TODO bring a way to kow how to add the data to the view.
    //     assuming by now that we always have at least layered views.
    //     this can be did by having a method that return the base abstract class(categorie)
    //     of the view. - RDE
    medAbstractLayeredView* view = dynamic_cast<medAbstractLayeredView*>(d->view);
    view->addLayer(data);
}

void medViewSplitableContainer::selfDestroy()
{
    this->~medViewSplitableContainer();
}

void medViewSplitableContainer::createDragLabels()
{
    QWidget *receiver;
    if(d->view)
        receiver = d->view->viewWidget();
    else
        receiver = d->emptyView;

    int w(receiver->width()), h(receiver->height());
    d->receiverQuarterWidth = w/4;
    d->receiverQuarterHeight = h/4;

    QColor labelColor(128, 65, 25);
    QGridLayout *receiverLayout = new QGridLayout;
    d->mainLayout->addLayout(receiverLayout, 1, 0, -1, -1);

    QImage hImg, vImg, mImg;
    if(h < w)
    {
//        hImg = QImage(w/2, h/4, QImage::Format_ARGB32);
//        vImg = QImage(h/4, h/2, QImage::Format_ARGB32);
        hImg = QImage(w, h/4, QImage::Format_ARGB32);
        vImg = QImage(h/4, h, QImage::Format_ARGB32);
    }
    else
    {
//        hImg = QImage(w/2, w/4, QImage::Format_ARGB32);
//        vImg = QImage(w/4, h/2, QImage::Format_ARGB32);
        hImg = QImage(w, w/4, QImage::Format_ARGB32);
        vImg = QImage(w/4, h, QImage::Format_ARGB32);
    }
    mImg = QImage(w/2, h/2, QImage::Format_ARGB32);

    hImg.fill(labelColor);
    vImg.fill(labelColor);
    mImg.fill(labelColor);

    d->northDragLabel = new QLabel;
    d->northDragLabel->setPixmap(QPixmap::fromImage(hImg));
    receiverLayout->addWidget(d->northDragLabel, 0, 1, 1, 1, Qt::AlignTop);

    d->eastDragLabel = new QLabel;
    d->eastDragLabel->setPixmap(QPixmap::fromImage(vImg));
    receiverLayout->addWidget(d->eastDragLabel, 1, 2, 1, 1, Qt::AlignRight);

    d->southDragLabel = new QLabel;
    d->southDragLabel->setPixmap(QPixmap::fromImage(hImg));
    receiverLayout->addWidget(d->southDragLabel, 2, 1, 1, 1, Qt::AlignBottom);

    d->westDragLabel = new QLabel;
    d->westDragLabel->setPixmap(QPixmap::fromImage(vImg));
    receiverLayout->addWidget(d->westDragLabel, 1, 0, 1, 1, Qt::AlignLeft);

    d->midDragLabel = new QLabel;
    d->midDragLabel->setPixmap(QPixmap::fromImage(mImg));
    receiverLayout->addWidget(d->midDragLabel, 1, 1, 1, 1, Qt::AlignCenter);

    d->northDragLabel->hide();
    d->eastDragLabel->hide();
    d->southDragLabel->hide();
    d->westDragLabel->hide();
    d->midDragLabel->hide();
}

void medViewSplitableContainer::destroyDragLabels()
{
    delete d->northDragLabel;
    d->northDragLabel = NULL;

    delete d->eastDragLabel;
    d->eastDragLabel = NULL;

    delete d->southDragLabel;
    d->southDragLabel = NULL;

    delete d->westDragLabel;
    d->westDragLabel = NULL;

    delete d->midDragLabel;
    d->midDragLabel = NULL;
}
