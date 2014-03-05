/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainer.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QUuid>

#include <medViewContainerManager.h>
#include <medAbstractView.h>
#include <medBoolParameter.h>
#include <medDataIndex.h>
#include <medAbstractData.h>
#include <medDataManager.h>
#include <medImageViewFactory.h>
#include <medLayeredViewFactory.h>
#include <medViewManager.h>
#include <medToolBox.h>
#include <medToolBoxHeader.h>
#include <medColorListParameter.h>
#include <medParameterPoolManager.h>
#include <medParameterPool.h>
#include <medViewContainerSplitter.h>

class medViewContainerPrivate
{
public:
    QUuid uuid;

    medAbstractView* view;
    medViewContainerSplitter* parent;

    bool selected;
    bool maximized;
    bool userSplittable;
    bool userClosable;
    bool multiLayer;

    QGridLayout* mainLayout;
    QHBoxLayout* toolBarLayout;

    QPushButton* vSplitButton;
    QPushButton* hSplitButton;
    QPushButton* closeContainerButton;

    QLabel *northDragLabel;
    QLabel *eastDragLabel;
    QLabel *southDragLabel;
    QLabel *westDragLabel;
    QLabel *midDragLabel;

    int receiverQuarterWidth;
    int receiverQuarterHeight;

    medToolBox *toolBox;

    medBoolParameter* maximizedParameter;
    medColorListParameter *poolSelector;

    ~medViewContainerPrivate()
    {
        if(view)
            delete view;
    }
};


medViewContainer::medViewContainer(medViewContainerSplitter *parent): QFrame(parent),
    d(new medViewContainerPrivate)
{
    d->parent = parent;

    d->uuid = QUuid::createUuid();
    medViewContainerManager::instance()->registerNewContainer(this);


    d->view = NULL;
    d->northDragLabel = NULL;
    d->eastDragLabel = NULL;
    d->westDragLabel = NULL;
    d->southDragLabel = NULL;
    d->midDragLabel = NULL;
    d->toolBox = NULL;

    d->closeContainerButton = new QPushButton(this);
    d->closeContainerButton->setIcon(QIcon(":/medGui/pixmaps/closebutton.png"));
    connect(d->closeContainerButton, SIGNAL(clicked()), this, SLOT(close()));
    d->closeContainerButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->closeContainerButton->setFocusPolicy(Qt::NoFocus);

    d->vSplitButton = new QPushButton(this);
    d->vSplitButton->setIcon(QIcon(":/medGui/pixmaps/splitbutton_vertical.png"));
    d->vSplitButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->vSplitButton->setFocusPolicy(Qt::NoFocus);
    connect(d->vSplitButton, SIGNAL(clicked()), this, SIGNAL(vSplitRequest()));
    d->hSplitButton = new QPushButton(this);
    d->hSplitButton->setIcon(QIcon(":/medGui/pixmaps/splitbutton_horizontal.png"));
    d->hSplitButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->hSplitButton->setFocusPolicy(Qt::NoFocus);
    connect(d->hSplitButton, SIGNAL(clicked()), this, SIGNAL(hSplitRequest()));

    // make it a parameter to get synch between state of the container and the maximized button.
    d->maximizedParameter = new medBoolParameter("maximied view", this);
    d->maximizedParameter->getPushButton()->setMaximumHeight(18);
    d->maximizedParameter->getPushButton()->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->maximizedParameter->getPushButton()->setFocusPolicy(Qt::NoFocus);
    QIcon maximizedIcon(":/icons/maximize.svg");
    maximizedIcon.addFile(":/icons/un_maximize.svg",
                        QSize(16,16),
                        QIcon::Normal,
                        QIcon::On);

    d->maximizedParameter->setIcon(maximizedIcon);
    d->maximized = true;
    connect(d->maximizedParameter, SIGNAL(valueChanged(bool)), this, SLOT(setMaximized(bool)));
    connect(this, SIGNAL(maximized(bool)), d->maximizedParameter, SLOT(setValue(bool)));
    d->maximizedParameter->setValue(false);
    d->maximizedParameter->hide();

    d->poolSelector = new medColorListParameter("Pool", this);
    d->poolSelector->addColor("");
    d->poolSelector->addColor("red", "1");
    d->poolSelector->addColor("green", "2");
    d->poolSelector->addColor("blue", "3");
    QPixmap icon(":/icons/link.svg");
    d->poolSelector->getLabel()->setPixmap(icon.scaled(15,15));
    connect(d->poolSelector, SIGNAL(valueChanged(QString)), this, SLOT(emitLinkRequested(QString)));

    QWidget* toolBar = new QWidget(this);
    toolBar->setObjectName("containerToolBar");
    toolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    d->toolBarLayout = new QHBoxLayout(toolBar);
    d->toolBarLayout->setContentsMargins(5,0,5,0);
    d->toolBarLayout->setSpacing(5);
    d->toolBarLayout->addWidget(d->poolSelector->getLabel(), 1, Qt::AlignRight);
    d->toolBarLayout->addWidget(d->poolSelector->getComboBox(), 0, Qt::AlignRight);
    d->toolBarLayout->addWidget(d->maximizedParameter->getPushButton(), 0, Qt::AlignRight);
    d->toolBarLayout->addWidget(d->vSplitButton, 0, Qt::AlignRight);
    d->toolBarLayout->addWidget(d->hSplitButton, 0, Qt::AlignRight);
    d->toolBarLayout->addWidget(d->closeContainerButton, 0, Qt::AlignRight);

    d->mainLayout = new QGridLayout(this);
    d->mainLayout->setContentsMargins(0, 0, 0, 0);
    d->mainLayout->setSpacing(0);
    d->mainLayout->addWidget(toolBar, 0, 0, Qt::AlignTop);

    this->setAcceptDrops(true);
    this->setUserSplittable(true);
    this->setUserClosable(true);
    this->setMultiLayered(true);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setMouseTracking(true);
    this->setSelected(false);
}

medViewContainer::~medViewContainer()
{
    removeInternView();

    //trick to 'inform' a parented splitter
    //"you're not my dad anymore!"
    this->setParent(NULL);

    medViewContainerManager::instance()->unregisterContainer(this);
    delete d;
}

QUuid medViewContainer::uuid() const
{
    return d->uuid;
}

medAbstractView* medViewContainer::view() const
{
    return d->view;
}

bool medViewContainer::isUserSplittable() const
{
    return d->userSplittable;
}

void medViewContainer::setUserSplittable(bool splittable)
{
    d->userSplittable = splittable;
    if(d->userSplittable)
    {
          d->hSplitButton->show();
          d->vSplitButton->show();
    }
    else
    {
        d->hSplitButton->hide();
        d->vSplitButton->hide();
    }
}

bool medViewContainer::isUserClosable() const
{
    return d->userClosable;
}

void medViewContainer::setUserClosable(bool closable)
{
    d->userClosable = closable;
    if(d->userClosable)
          d->closeContainerButton->show();
    else
        d->closeContainerButton->hide();
}

bool medViewContainer::isMultiLayered() const
{
    return d->multiLayer;
}

void medViewContainer::setMultiLayered(bool multilayer)
{
    d->multiLayer = multilayer;
}

void medViewContainer::setContainerParent(medViewContainerSplitter *splitter)
{
    d->parent = splitter;
}

void medViewContainer::setView(medAbstractView *view)
{
    if(d->view)
    {
        removeInternView();
    }
    if(view)
    {
        d->view = view;
        connect(d->view, SIGNAL(destroyed()), this, SLOT(removeInternView()));
        connect(d->view, SIGNAL(selectedRequest(bool)), this, SLOT(setSelected(bool)));
        if(medAbstractLayeredView* layeredView = dynamic_cast<medAbstractLayeredView*>(view))
            connect(layeredView, SIGNAL(currentLayerChanged()), this, SIGNAL(currentLayerChanged()));

        if( d->view->toolBarWidget() )
        {
            d->view->toolBarWidget()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            d->toolBarLayout->insertWidget(0, d->view->toolBarWidget());
        }

        d->maximizedParameter->show();
        d->mainLayout->addWidget(d->view->viewWidget(), 1, 0, 1, 1);
        d->view->viewWidget()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        d->view->viewWidget()->show();
        d->toolBox = new medToolBox;
        d->toolBox->setTitle("Container settings");
        d->toolBox->header()->hide();

        QString tooltip = QString(tr("Link View properties ("));
        foreach(medAbstractParameter *param, d->view->navigatorsParameters())
            tooltip += param->name() + ", ";
        tooltip += ")";
        d->poolSelector->setToolTip(tooltip);

        emit viewChanged();
    }
}

bool medViewContainer::isSelected() const
{
    return d->selected;
}

void medViewContainer::setSelected(bool selec)
{
    if(selec == d->selected)
    {
        if(QApplication::keyboardModifiers() == Qt::ControlModifier)
        {
            this->setSelected(!selec);
        }
        //clear focus in order to select/unselect successively twice the same container
        this->clearFocus();
        return;
    }

    d->selected = selec;
    if(d->selected)
    {
        emit containerSelected(d->uuid);
    }
    else
    {
        emit containerUnSelected(d->uuid);
        this->unHighlight();
    }
    //clear focus in order to select/unselect successively twice the same container
    this->clearFocus();
}

void medViewContainer::highlight(QString color)
{
    // TODO: recomputeStyleSheet deosn't seem to work here
    // temporary setStyleSheet to update the border color
    QString styleSheet = "medViewContainer {border:2px solid " + color + ";}";
    this->setStyleSheet(styleSheet);
    if(d->view)
    {
        d->view->viewWidget()->updateGeometry();
        d->view->viewWidget()->update();

    }
}

void medViewContainer::unHighlight()
{
    this->setStyleSheet("medViewContainer {border:2px solid #909090;}");
    if(d->view)
    {
        d->view->viewWidget()->updateGeometry();
        d->view->viewWidget()->update();
    }
}

void medViewContainer::setUnSelected(bool unSelected)
{
    this->setSelected(!unSelected);
}

void medViewContainer::setMaximized(bool maxi)
{
    if(d->maximized == maxi)
        return;

    d->maximized = maxi;

    if(maxi)
    {
        d->vSplitButton->hide();
        d->hSplitButton->hide();
    }
    else if(d->userSplittable)
    {
        d->vSplitButton->show();
        d->hSplitButton->show();
    }
    emit maximized(maxi);
    emit maximized(d->uuid, maxi);
}

bool medViewContainer::isMaximized() const
{
    return d->maximized;
}

void medViewContainer::removeView()
{
    if(!d->view)
        return;

    delete d->view;
    // removeInternView should be called, so no need to set d->view to NULL
    // or whatecer else
}

void medViewContainer::removeInternView()
{
    if(d->view && d->view->viewWidget())
      d->view->viewWidget()->hide();

    d->view = NULL;
    d->toolBox = NULL;
    d->maximizedParameter->hide();

    emit viewRemoved();
}

void medViewContainer::focusInEvent(QFocusEvent *event)
{
    if(event->reason() == Qt::ActiveWindowFocusReason)
        return;

    this->setSelected(true);
    QWidget::focusInEvent(event);
}

void medViewContainer::recomputeStyleSheet()
{
    this->style()->unpolish(this);
    this->style()->polish(this);
}

void medViewContainer::dragEnterEvent(QDragEnterEvent *event)
{
    this->createDragLabels();
    event->acceptProposedAction();
}

void medViewContainer::dragMoveEvent(QDragMoveEvent *event)
{
    if(d->userSplittable)
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
    }

    event->acceptProposedAction();
}

void medViewContainer::dragLeaveEvent(QDragLeaveEvent *event)
{
    this->destroyDragLabels();
    event->accept();
}

void medViewContainer::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    medDataIndex index = medDataIndex::readMimeData(mimeData);
    if(!index.isValidForSeries())
        return;

    if(d->userSplittable)
    {
        this->destroyDragLabels();

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
    }
    else
        this->addData(medDataManager::instance()->data(index));

    event->acceptProposedAction();
}

void medViewContainer::addData(medAbstractData *data)
{
    if(!data)
        return;

    if(!d->multiLayer)
        delete d->view;

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
    emit viewContentChanged();
}

void medViewContainer::createDragLabels()
{
    // minus 4 because we don't want to hide  the borders
    int w(this->width() - 4), h(this->height() - 4);
    d->receiverQuarterWidth = w/4;
    d->receiverQuarterHeight = h/4;

    QColor labelColor(80, 80, 80);
    QGridLayout *receiverLayout = new QGridLayout;
    d->mainLayout->addLayout(receiverLayout, 0, 0, -1, -1);

    QImage hImg, vImg, mImg;
    if(h < w)
    {
        hImg = QImage(w, h/4, QImage::Format_ARGB32);
        vImg = QImage(h/4, h, QImage::Format_ARGB32);
    }
    else
    {
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

void medViewContainer::destroyDragLabels()
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


void medViewContainer::emitLinkRequested(QString pool)
{
    if(pool == "")
    {
        emit unlinkRequested(d->uuid);
    }
    else
    {
        emit linkRequested(d->uuid, pool);
    }
}

void medViewContainer::link(QString pool)
{
    if(!d->view)
        return;

    if(pool!="")
        unlink();

    d->poolSelector->blockSignals(true);
    d->poolSelector->setCurrentColor(pool);
    d->poolSelector->blockSignals(false);

    foreach(medAbstractParameter *param, d->view->navigatorsParameters())
    {
        medParameterPoolManager::instance()->linkParameter(param, pool);
    }
}

void medViewContainer::unlink()
{
    if(!d->view)
        return;

    d->poolSelector->blockSignals(true);
    d->poolSelector->setCurrentColor("");
    d->poolSelector->blockSignals(false);

    foreach(medAbstractParameter *param, d->view->navigatorsParameters())
    {
        medParameterPoolManager::instance()->unlinkParameter(param);
    }
}


medViewContainer * medViewContainer::splitHorizontally()
{
    if(!d->parent)
        return NULL;

    return d->parent->splitHorizontally(this);
}

medViewContainer * medViewContainer::splitVertically()
{
    if(!d->parent)
        return NULL;

    return d->parent->splitVertically(this);
}

medViewContainer *medViewContainer::split(Qt::AlignmentFlag alignement)
{
    if(!d->parent)
        return NULL;

    return d->parent->split(this, alignement);
}

void medViewContainer::closeEvent(QCloseEvent * event)
{
    this->~medViewContainer();
}
