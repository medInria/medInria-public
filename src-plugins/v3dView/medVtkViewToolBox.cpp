#include <medVtkViewToolBox.h>

#include <medAbstractData.h>
#include <medToolBoxFactory.h>
#include <medVtkView.h>
#include <medVtkView2dParamsToolBox.h>
#include <medVtkView3dParamsToolBox.h>
#include <medToolBoxHeader.h>
#include <medSettingsManager.h>
#include <medMetaDataKeys.h>
#include <medAbstractVtkViewInteractor.h>
#include <medLayerItemWidget.h>


class medVtkViewToolBoxPrivate
{
public:

    medVtkView                      * vtkView;
    QMultiMap<medVtkView*, medAbstractParameter*> viewParamMap;
    QWidget                         * mainOrientationWidget;
    QHBoxLayout                     * mainOrientationLayout;
    medVtkView2dParamsToolBox       * view2dToolBox;
    medVtkView3dParamsToolBox       * view3dToolBox;

    QListWidget                     * layersListWidget;
    QFormLayout                     * interactorsParamsLayout;
    QList<medVtkView*>                selectedViewsList;
    QMap<QString,medVtkView*>         viewMap;
    QHash<QListWidgetItem*, QPair<QString,int> > layerItemHash;

    QList<medAbstractParameter*>      layerParamList;
    QMap<QString, medAbstractParameter*> paramMap;

    medParameterPool                  layerParamPool;
    medParameterPool                  viewParamPool;

};

medVtkViewToolBox::medVtkViewToolBox(QWidget *parent)
    : medToolBox(parent), d(new medVtkViewToolBoxPrivate)
{
    d->vtkView = 0;

    this->setTitle(tr("Views management"));

    d->mainOrientationWidget = new QWidget(this);
    d->mainOrientationLayout = new QHBoxLayout;
    d->mainOrientationWidget->setLayout(d->mainOrientationLayout);

    d->view2dToolBox = new medVtkView2dParamsToolBox;
    d->view2dToolBox->header()->hide();
    d->view3dToolBox = new medVtkView3dParamsToolBox;
    d->view3dToolBox->header()->hide();

    d->layersListWidget = new QListWidget(this);
    d->layersListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QWidget *interactorsParamsWidget = new QWidget(this);
    d->interactorsParamsLayout = new QFormLayout(interactorsParamsWidget);
    d->interactorsParamsLayout->setLabelAlignment(Qt::AlignLeft);
    //d->interactorsParamsLayout->setContentsMargins(0,0,0,0);

    d->viewMap = QMap<QString, medVtkView*>();

    this->addWidget(d->mainOrientationWidget);
    this->addWidget(d->view2dToolBox);
    this->addWidget(d->view3dToolBox);

    this->addWidget(d->layersListWidget);
    this->addWidget(interactorsParamsWidget);

    connect(d->layersListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateLayerParameters()));

}


medVtkViewToolBox::~medVtkViewToolBox()
{
    d->viewParamMap.clear();

    d->selectedViewsList.clear();
    d->viewMap.clear();
    d->layerItemHash.clear();

    d->layerParamList.clear();
    d->paramMap.clear();
}

bool medVtkViewToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medVtkViewToolBox>("medVtkViewToolBox","medVtkViewToolBox",
                                                                                "View management toolbox", QStringList() << "view");
}

QList<medAbstractParameter*> medVtkViewToolBox::getViewParameters(medVtkView* view)
{
    QList<medAbstractParameter*> viewParams;
    viewParams.append(d->viewParamMap.values(view));
    viewParams.append(d->view2dToolBox->getParameters(view));
    viewParams.append(d->view3dToolBox->getParameters(view));
    return viewParams;

}

void medVtkViewToolBox::update(dtkAbstractView * view)
{
    d->view2dToolBox->update(view);
    d->view3dToolBox->update(view);

    // Hide this toolbox when no views
    if(!view)
    {
        this->hide();
        d->vtkView = 0;
        return;
    }
    else
        this->show();

    if (d->vtkView)
        d->vtkView->disconnect(this);

    d->vtkView = qobject_cast<medVtkView*>(view);

    if( !d->viewParamMap.keys().contains(d->vtkView))
      setupOrientationParamters(view);

    QList<medAbstractParameter*> allViewParams = d->viewParamMap.values();
    foreach(medAbstractParameter *param, allViewParams)
        param->hide();

    QList<medAbstractParameter*> viewParams = getViewParameters(d->vtkView);
    foreach(medAbstractParameter *param, viewParams)
        param->show();


    setViewMode(display2d);

    if(d->vtkView)
    {
        addView(d->vtkView);

        updateViews();

        connect(d->vtkView, SIGNAL(dataAdded(dtkAbstractData*, int)), this, SLOT(updateViews()));
        connect(d->vtkView, SIGNAL(dataRemoved(dtkAbstractData*, int)), this, SLOT(updateViews()));
        connect(d->vtkView, SIGNAL(closed()), this ,SLOT(removeView()));
        connect(d->vtkView, SIGNAL(closing()), this ,SLOT(removeView()));

    }
}

void medVtkViewToolBox::setupOrientationParamters(dtkAbstractView * view)
{
    // Orientation

    medActionParameter *axialParam = new medActionParameter("Axial", view);
    axialParam->setIcon( QIcon (":/icons/AxialIcon.png"));
    axialParam->setCheckable(true);
    axialParam->setIconSize(45,45);
    axialParam->hideLabel();

    medActionParameter *coronalParam = new medActionParameter("Coronal", view);
    coronalParam->setIcon(QIcon (":/icons/CoronalIcon.png"));
    coronalParam->setCheckable(true);
    coronalParam->setIconSize(45,45);
    coronalParam->hideLabel();

    medActionParameter *sagitalParam = new medActionParameter("Sagital", view);
    sagitalParam->setIcon(QIcon (":/icons/SagittalIcon.png"));
    sagitalParam->setCheckable(true);
    sagitalParam->setIconSize(45,45);
    sagitalParam->hideLabel();

    medActionParameter *view3DParam = new medActionParameter("3D", view);
    view3DParam->setIcon(QIcon (":/icons/3DIcon.png"));
    view3DParam->setCheckable(true);
    view3DParam->setIconSize(45,45);
    view3DParam->hideLabel();

    medActionParameterGroup *orientationGroup = new medActionParameterGroup (this);
    orientationGroup->addActionParameter ( axialParam );
    orientationGroup->addActionParameter ( coronalParam );
    orientationGroup->addActionParameter ( sagitalParam );
    orientationGroup->addActionParameter ( view3DParam );
    orientationGroup->setExclusive (true);


    // Layouts
    d->mainOrientationLayout->addWidget(axialParam->getWidget());
    d->mainOrientationLayout->addWidget(coronalParam->getWidget());
    d->mainOrientationLayout->addWidget(sagitalParam->getWidget());
    d->mainOrientationLayout->addWidget(view3DParam->getWidget());

    // Connection
    connect(axialParam,SIGNAL(triggered(bool)),this,SLOT(setAxial(bool)));
    connect(coronalParam,SIGNAL(triggered(bool)),this,SLOT(setCoronal(bool)));
    connect(sagitalParam,SIGNAL(triggered(bool)),this,SLOT(setSagittal(bool)));
    connect(view3DParam,SIGNAL(triggered(bool)),this,SLOT(setView3D(bool)));

    d->viewParamMap.insert(d->vtkView, axialParam);
    d->viewParamMap.insert(d->vtkView, coronalParam);
    d->viewParamMap.insert(d->vtkView, sagitalParam);
    d->viewParamMap.insert(d->vtkView, view3DParam);

    axialParam->setChecked(true);

}

void medVtkViewToolBox::setAxial(bool checked)
{
    if (checked && d->vtkView)
    {
        d->vtkView->setOrientation( "Axial" );
        d->vtkView->update();
        setViewMode(display2d);
    }
}

void medVtkViewToolBox::setSagittal(bool checked)
{
    if (checked && d->vtkView)
    {
        d->vtkView->setOrientation ( "Sagittal" );
        d->vtkView->update();
        setViewMode(display2d);
    }
}

void medVtkViewToolBox::setCoronal(bool checked)
{
    if (checked && d->vtkView)
    {
        d->vtkView->setOrientation ( "Coronal" );
        d->vtkView->update();
        setViewMode(display2d);
    }
}

void medVtkViewToolBox::setView3D(bool checked)
{
    if (checked && d->vtkView)
    {
        d->vtkView->set3DMode( "3DMode" );
        d->vtkView->setOrientation ( "3D" );
        d->vtkView->update();
        setViewMode(display3d);
    }
}

void medVtkViewToolBox::setViewMode(viewMode mode)
{
    if(mode==display2d)
    {
        d->view2dToolBox->show();
        d->view3dToolBox->hide();
    }
    else if(mode==display3d)
    {
        d->view2dToolBox->hide();
        d->view3dToolBox->show();
    }
}


void medVtkViewToolBox::addView(medVtkView *view)
{
    if(!view)
        return;

    if( QApplication::keyboardModifiers() != Qt::ControlModifier )
    {
        d->selectedViewsList.clear();
        d->viewMap.clear();
    }

    if(!d->selectedViewsList.contains(view))
    {
        d->selectedViewsList.append(view);

        d->viewMap.insert(view->name(), view);
    }
}


void medVtkViewToolBox::updateViews()
{
    updateLayerListWidget(d->selectedViewsList);

    updateViewPool(d->selectedViewsList);
}


void medVtkViewToolBox::updateLayerListWidget(QList<medVtkView*> vtkViews)
{
    resetList();

    if (vtkViews.isEmpty()) {return;}

    d->layersListWidget->blockSignals(true);

    int lastLayer = -1;

    foreach(medVtkView *view, vtkViews)
    {
        int nbLayers = view->layersCount();

        for (int i  = 0; i < nbLayers; i++) {

            medAbstractData * layerData = qobject_cast<medAbstractData*>(view->layerData(i));

            if(layerData)
            {
                QString thumbPath = medMetaDataKeys::SeriesThumbnail.getFirstValue(layerData,":icons/layer.png");
                QString layerName = medMetaDataKeys::SeriesDescription.getFirstValue(layerData,"<i>no name</i>");

                QList<medAbstractVtkViewInteractor*> interactors;
                foreach(dtkAbstractViewInteractor* i, view->interactors()) {
                    medAbstractVtkViewInteractor * interactor = qobject_cast<medAbstractVtkViewInteractor*>(i);
                    if ( interactor && interactor->isDataTypeHandled(layerData->identifier()))
                        interactors.append(interactor);
                }

                QListWidgetItem * item = new QListWidgetItem(d->layersListWidget);
                medLayerItemWidget * widget = new medLayerItemWidget(layerName, thumbPath, layerData, interactors, d->layersListWidget);
                item->setSizeHint(widget->sizeHint());

                d->layersListWidget->addItem(item);
                d->layerItemHash.insert(item, QPair<QString, int>(view->name(), i));
                d->layersListWidget->setItemWidget(item, widget);

                lastLayer++;

                connect(widget, SIGNAL(deletionRequested()), this, SLOT(removeLayer()));
            }
        }
    }

    d->layersListWidget->blockSignals(false);

    if( lastLayer > -1 )
    {
        d->layersListWidget->clearSelection();
        d->layersListWidget->item(lastLayer)->setSelected(true);
    }
}


void medVtkViewToolBox::updateLayerParameters()
{
    clearParams();

    int nbLayers = d->layersListWidget->count();

    QMultiMap<medVtkView*, int> selectedLayers;

    for (int i  = 0; i < nbLayers; i++)
    {
        QListWidgetItem * item = d->layersListWidget->item(i);
        if( item->isSelected() )
        {
            QPair<QString, int> itemPair = d->layerItemHash.value(item);
            QString viewName = itemPair.first;
            int layer = itemPair.second;
            selectedLayers.insert(d->viewMap.value(viewName), layer);
        }
    }

    updateLayerParameters(selectedLayers);
}


void medVtkViewToolBox::updateLayerParameters(QMultiMap<medVtkView*, int> selectedLayers)
{
    clearParams();

    d->layerParamPool.clear();

    QMap<QString, medAbstractParameter*> commonParamList;
    commonParamList.setInsertInOrder(true);

    QMapIterator<medVtkView*, int> i(selectedLayers);

    int j = 0;

    while (i.hasNext())
    {
        i.next();

        medVtkView* view =  i.key();
        int layer = i.value();

        medAbstractData * layerData = qobject_cast<medAbstractData*>(view->layerData(layer));

        if(!layerData)
            return;

        foreach(dtkAbstractViewInteractor* i, view->interactors())
        {
            medAbstractVtkViewInteractor * interactor = qobject_cast<medAbstractVtkViewInteractor*>(i);
            if ( interactor && interactor->isDataTypeHandled(layerData->identifier()))
            {
                QList<medAbstractParameter*> paramList = interactor->getParameters(view->layerData(layer));

                foreach(medAbstractParameter *param, paramList)
                {
                    bool found = false;

                    foreach(medAbstractParameter *existingParam, d->layerParamList)
                    {
                        if(param->match(existingParam))
                        {
                            found = true;
                            d->layerParamPool.append(param);
                            d->layerParamPool.append(existingParam);

                            if(!commonParamList.contains(param->name()))
                              commonParamList.insert(param->name(), param);

                            break;
                        }
                    }
                    if(!found)
                    {
                        d->layerParamList.append(param);
                        d->paramMap.insert(param->name(), param);

                        j++;
                    }
                }
            }
        }
    }

    QList<medAbstractParameter *> displayedParam;
    if(selectedLayers.count() == 1)
        displayedParam = d->layerParamList;
    else displayedParam = commonParamList.values();

    foreach(medAbstractParameter *param, displayedParam)
    {
        param->show();
        d->interactorsParamsLayout->addRow(param->getLabel(),param->getWidget());
    }

}


void medVtkViewToolBox::updateViewPool(QList<medVtkView*> selectedViews)
{
    d->viewParamPool.clear();

    if(selectedViews.size()>0)
    {
        foreach(medVtkView* view, selectedViews)
        {
            QList<medAbstractParameter*> paramList = getViewParameters(view);
            foreach(medAbstractParameter *param, paramList)
            {
                d->viewParamPool.append(param);
            }
        }
    }
}


void medVtkViewToolBox::resetList()
{
    d->layersListWidget->blockSignals(true);
    d->layersListWidget->clear();
    d->layersListWidget->blockSignals(false);

    d->layerItemHash.clear();
}


void medVtkViewToolBox::clearParams()
{
    foreach(medAbstractParameter *param, d->layerParamList)
        param->hide();

    d->layerParamList.clear();

    QLayoutItem* item;
    while ( ( item = d->interactorsParamsLayout->takeAt( 0 ) ) != NULL )
    {
        item->widget()->close();
        //delete item;
    }
}


void medVtkViewToolBox::removeView()
{
    medVtkView *sender = dynamic_cast<medVtkView*>(QObject::sender());
    if(sender)
    {
        d->viewMap.remove(sender->name());
        d->selectedViewsList.removeOne(sender);
    }
}


void medVtkViewToolBox::removeLayer()
{
    medLayerItemWidget *sender = dynamic_cast<medLayerItemWidget*>(QObject::sender());

    if(sender)
    {
        QList<QListWidgetItem*> items = d->layersListWidget->selectedItems();
        foreach(QListWidgetItem* item, items)
        {
          d->layersListWidget->removeItemWidget(item);
        }

        d->vtkView->removeLayer(sender->dataItem());
    }

}





