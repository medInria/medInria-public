/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.

 See LICENSE.txt for details in the root of the sources or:
 https://github.com/medInria/medInria-public/blob/master/LICENSE.txt

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.

=========================================================================*/

#include <medAbstractView.h>

#include <QPair>
#include <QUndoStack>
#include <QMouseEvent>

#include <dtkCoreSupport/dtkSmartPointer>

#include <medAbstractData.h>
#include <medAbstractNavigator.h>
#include <medAbstractViewNavigator.h>
#include <medBoolGroupParameterL.h>
#include <medStringListParameterL.h>
#include <medViewFactory.h>

/**
 * @fn QWidget* medAbstractView::viewWidget()
 * @brief Return the widget containing the actual view object.
 */

/**
 * @fn medViewBackend * medAbstractView::backend() const
 * @brief Return the backend of the view
 */

/**
 * @fn QImage medAbstractView::buildThumbnail(const QSize &size)
 * @brief Build and return a thumbnail of the view of the specified size
 */


class medAbstractViewPrivate
{
public:
    bool    closable;

    medAbstractViewInteractor* primaryInteractor;
    QList<medAbstractInteractor*>  extraInteractors;

    medAbstractViewNavigator* primaryNavigator;
    QList<medAbstractNavigator*> extraNavigators;

    // toolboxes
    QPointer<QWidget> toolBarWidget;
    QPointer<QWidget> navigatorWidget;
    QPointer<QWidget> mouseInteractionWidget;

    // Smart pointed.
    // dtkSmartPointer should only be used in views, process and dataManager.
    dtkSmartPointer<medAbstractData> data;

    QUndoStack *undoStack;
};


medAbstractView::medAbstractView(QObject* parent) :d (new medAbstractViewPrivate)
{
    this->setParent(parent);
    d->closable = true;

    d->data = nullptr;
    d->primaryInteractor = nullptr;
    d->primaryNavigator = nullptr;

    d->toolBarWidget = nullptr;
    d->navigatorWidget = nullptr;
    d->mouseInteractionWidget = nullptr;

    d->undoStack = new QUndoStack(this);
}

medAbstractView::~medAbstractView( void )
{
    emit closed();
    delete d;
    d = nullptr;
}

/**
 * Add data to the view. Will replace the existing one if there is one.
 */
void medAbstractView::addData(medAbstractData *data)
{
    if(!data)
    {
        qWarning() << "Attempt to add a null data to the view: " << this;
        return;
    }

    if(data == d->data)
    {
        qDebug() << "Attempt to set twice the same data to the view: " << this;
        return;
    }

    d->data = data;

    bool initSuccess = this->initialiseInteractors(data);
    if(!initSuccess)
    {
        d->data = nullptr;
        return;
    }

    this->reset();
}

/**
 * Remove the current data from the view
 */
void medAbstractView::clear()
{
    this->removeInteractors(d->data);
    d->data = nullptr;
}

void medAbstractView::removeInteractors(medAbstractData *data)
{
    Q_UNUSED(data);
    delete d->primaryInteractor;
    d->primaryInteractor = nullptr;
    d->extraInteractors.clear();
}

bool medAbstractView::initialiseInteractors(medAbstractData *data)
{
    // primary
    medViewFactory* factory = medViewFactory::instance();
    QStringList primaryInt = factory->interactorsAbleToHandle(this->identifier(), data->identifier());
    if(primaryInt.isEmpty())
    {
        qWarning() << "Unable to find any primary interactor for: " << this->identifier() << "and" << data->identifier();
        return false;
    }
    else
    {
        medAbstractViewInteractor* interactor = factory->createInteractor<medAbstractViewInteractor>(primaryInt.first(), this);
        interactor->setInputData(data);
        d->primaryInteractor = interactor;
        connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
    }

    // extra
    QStringList extraInt = factory->additionalInteractorsAbleToHandle(this->identifier(), data->identifier());
    if(!extraInt.isEmpty())
    {
        QList<medAbstractInteractor*> extraIntList;
        for(QString i : extraInt)
        {
            medAbstractInteractor* interactor = factory->createAdditionalInteractor(i, this);
            interactor->setInputData(data);
            extraIntList << interactor;
            connect(this, SIGNAL(orientationChanged()), interactor, SLOT(updateWidgets()));
        }
        d->extraInteractors = extraIntList;
    }
    return true;
}

bool medAbstractView::initialiseNavigators()
{
    // primary
    medViewFactory* factory = medViewFactory::instance();
    QStringList primaryNav = factory->navigatorsAbleToHandle(this->identifier());
    if(primaryNav.isEmpty())
    {
        qWarning() << "Unable to find any primary navigator for: " << this->identifier();
        return false;
    }
    else
    {
        d->primaryNavigator = factory->createNavigator<medAbstractViewNavigator>(primaryNav.first(), this);
        connect(this, SIGNAL(orientationChanged()), d->primaryNavigator, SLOT(updateWidgets()));
    }

    // extra
    QStringList extraNav = factory->additionalNavigatorsAbleToHandle(this->identifier());
    if(!extraNav.isEmpty())
    {
        for(QString n : extraNav)
        {
            medAbstractNavigator *nav = factory->createAdditionalNavigator(n, this);
            connect(this, SIGNAL(orientationChanged()), nav, SLOT(updateWidgets()));
            d->extraNavigators << nav;
        }
    }
    return true;
}

medAbstractViewInteractor* medAbstractView::primaryInteractor(medAbstractData* data)
{
    Q_UNUSED(data);
    return d->primaryInteractor;
}

QList<medAbstractInteractor*> medAbstractView::extraInteractors(medAbstractData* data)
{
    Q_UNUSED(data);
    return d->extraInteractors;
}

medAbstractViewInteractor* medAbstractView::primaryInteractor()
{
    return d->primaryInteractor;
}

QList<medAbstractInteractor*> medAbstractView::extraInteractors()
{
    return d->extraInteractors;
}

medAbstractViewNavigator* medAbstractView::primaryNavigator()
{
    return d->primaryNavigator;
}

QList<medAbstractNavigator*> medAbstractView::extraNavigators()
{
    return d->extraNavigators;
}

/**
 * Return all navigators (primary + extra)
 */
QList<medAbstractNavigator*> medAbstractView::navigators()
{
    QList<medAbstractNavigator*> navigatorsList;

    if(this->primaryNavigator())
        navigatorsList << this->primaryNavigator();

    if(!d->extraNavigators.isEmpty())
        navigatorsList << d->extraNavigators;

    return navigatorsList;
}

/**
 * Return all interactors (primary + extra)
 */
QList<medAbstractInteractor*> medAbstractView::interactors()
{
    QList<medAbstractInteractor*> interactorsList;

    if(this->primaryInteractor(d->data))
        interactorsList << this->primaryInteractor(d->data);

    if(!this->extraInteractors(d->data).isEmpty())
        interactorsList << this->extraInteractors(d->data);

    return interactorsList;
}

medDoubleParameterL* medAbstractView::zoomParameter()
{
    medAbstractViewNavigator* pNavigator = this->primaryNavigator();
    if(!pNavigator)
    {
        return nullptr;
    }

    return pNavigator->zoomParameter();
}

medAbstractVector2DParameterL* medAbstractView::panParameter()
{
    medAbstractViewNavigator* pNavigator = this->primaryNavigator();
    if(!pNavigator)
    {
        return nullptr;
    }

    return pNavigator->panParameter();
}

bool medAbstractView::eventFilter(QObject * obj, QEvent * event)
{
    if(obj == this->viewWidget())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
            if(mouseEvent && mouseEvent->button() == Qt::LeftButton)
              emit selectedRequest(true);
        }
        else if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
            if( keyEvent && keyEvent->matches(QKeySequence::Undo) )
              d->undoStack->undo();
            if( keyEvent && keyEvent->matches(QKeySequence::Redo) )
              d->undoStack->redo();
        }

    }
    return dtkAbstractView::eventFilter(obj, event);
}

void medAbstractView::setUpViewForThumbnail()
{
    medAbstractViewInteractor *primaryInteractor = this->primaryInteractor();
    if(!primaryInteractor)
    {
        QString msg = "Unable to find any current primary interactor for view "  + this->identifier();
        qWarning() << msg;
    }

    else
        this->primaryInteractor()->setUpViewForThumbnail();
}

/**
 * Return the list of parameters that can be linked
 */
QList<medAbstractParameterL*> medAbstractView::linkableParameters()
{
    QList<medAbstractParameterL*>  params;
    params.append(this->primaryNavigator()->linkableParameters());

    for(medAbstractNavigator* nav :  this->extraNavigators())
    {
        params.append(nav->linkableParameters());
    }
    return params;
}

QImage medAbstractView::generateThumbnail(const QSize &size)
{
    setUpViewForThumbnail();
    return buildThumbnail(size);
}

void medAbstractView::setOffscreenRendering(bool /*isOffscreen*/)
{
    // nothing by default
}

QWidget* medAbstractView::toolBarWidget()
{
    if(d->toolBarWidget.isNull())
    {
        for(medAbstractInteractor *interactor : this->interactors())
        {
            if(!interactor)
            {
                continue;
            }

            QWidget* widget = interactor->toolBarWidget();
            if(widget)
            {
                if(!d->toolBarWidget)
                {
                    d->toolBarWidget = new QWidget;
                    QHBoxLayout *tbLayout = new QHBoxLayout(d->toolBarWidget);
                    tbLayout->setContentsMargins(0, 0, 0, 0);
                    tbLayout->setSpacing(0);
                }
                d->toolBarWidget->layout()->addWidget(widget);
            }
        }

        for(medAbstractNavigator *navigator : this->navigators())
        {
            if(!navigator)
            {
                continue;
            }

            QWidget* widget = navigator->toolBarWidget();
            if(widget)
            {
                if(!d->toolBarWidget)
                {
                    d->toolBarWidget = new QWidget;
                    QHBoxLayout *tbLayout = new QHBoxLayout(d->toolBarWidget);
                    tbLayout->setContentsMargins(0, 0, 0, 0);
                    tbLayout->setSpacing(0);
                }
                d->toolBarWidget->layout()->addWidget(widget);
            }
        }
    }

    return d->toolBarWidget;
}

/**
 * Build and return the widget made of all the widgets used to navigate through the view
 */
QWidget* medAbstractView::navigatorWidget()
{
    if(d->navigatorWidget.isNull())
    {
        d->navigatorWidget = new QWidget;
        QVBoxLayout* navigatorLayout = new QVBoxLayout(d->navigatorWidget);

        navigatorLayout->addWidget(primaryNavigator()->toolBoxWidget());
        for(medAbstractNavigator* navigator : this->extraNavigators())
        {
            navigatorLayout->addWidget(navigator->toolBoxWidget());
        }
    }

    return d->navigatorWidget;
}

/**
 * Build and return the widget made of all the widgets used to control mouse interactions
 */
QWidget* medAbstractView::mouseInteractionWidget()
{
    // We need to reconstruct mouseInteractionWidget every time
    // because a new inserted data can have brought new mouse interaction parameters
    if(!d->mouseInteractionWidget.isNull())
    {
        delete d->mouseInteractionWidget;
    }

    d->mouseInteractionWidget = new QWidget;

    QList<medBoolParameterL*> params;

    for(medAbstractInteractor* interactor : this->interactors())
    {
        params.append(interactor->mouseInteractionParameters());
    }

    for(medAbstractNavigator* navigator : this->navigators())
    {
        params.append(navigator->mouseInteractionParameters());
    }

    medBoolGroupParameterL *groupParam = new medBoolGroupParameterL("Mouse Interaction", this);
    groupParam->setPushButtonDirection(QBoxLayout::LeftToRight);
    for(medBoolParameterL* param : params)
    {
        groupParam->addParameter(param);
    }

    d->mouseInteractionWidget = groupParam->getPushButtonGroup();
    d->mouseInteractionWidget->setContentsMargins(4, 4, 4, 4);

    return d->mouseInteractionWidget;
}

QUndoStack* medAbstractView::undoStack() const
{
    return d->undoStack;
}

QMainWindow* medAbstractView::mainWindow()
{
    return nullptr;
}

void medAbstractView::restoreState(QDomElement *element)
{
}
