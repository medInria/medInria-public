//#include <medViewLayersToolBox.h>
//#include <medToolBoxFactory.h>

//#include <QWidgetList>

//#include <medLayerItemWidget.h>
//#include <medMetaDataKeys.h>
//#include <medVtkView.h>
//#include <medAbstractData.h>
//#include <medAbstractVtkViewInteractor.h>
//#include <medParameter.h>
//#include <medVtkViewToolBox.h>
//#include <medToolBoxHeader.h>
//#include <medViewContainer.h>

//#include <QPushButton>

//class medViewLayersToolBoxPrivate
//{
//public:

//    QListWidget * layersListWidget;
//    QFormLayout * interactorsParamsLayout;
//    medVtkView  * vtkView;
//    QList<QWidget*> paramWidgetList;
//    QList<medVtkView*> viewList;
//    QMap<QString,medVtkView*> viewMap;
//    QHash<QListWidgetItem*, QPair<QString,int> > layerItemHash;

//    QList<medAbstractParameter*> paramList;
//    QMap<QString, medAbstractParameter*> paramMap;

//    medParameterPool layerParamPool;
//    medParameterPool viewParamPool;
//    medVtkViewToolBox *viewParamsToolBox;
//};




//medViewLayersToolBox::medViewLayersToolBox(QWidget *parent)
//    : medToolBox(parent), d(new medViewLayersToolBoxPrivate)
//{
//    this->setTitle(tr("Layers management"));

//    d->vtkView = 0;

//    d->layersListWidget = new QListWidget(this);
//    d->layersListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

//    QWidget *interactorsParamsWidget = new QWidget(this);
//    d->interactorsParamsLayout = new QFormLayout(interactorsParamsWidget);
//    d->interactorsParamsLayout->setLabelAlignment(Qt::AlignLeft);

//    d->viewParamsToolBox = new medVtkViewToolBox();
//    d->viewParamsToolBox->header()->hide();

//    this->addWidget(d->viewParamsToolBox);
//    this->addWidget(d->layersListWidget);
//    this->addWidget(interactorsParamsWidget);

//    d->paramWidgetList = QList<QWidget*>();
//    d->viewMap = QMap<QString, medVtkView*>();

//    connect(d->layersListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateParameters()));
//}




//bool medViewLayersToolBox::registered()
//{
//    return medToolBoxFactory::instance()->registerToolBox<medViewLayersToolBox>("medViewLayersToolBox","medViewLayersToolBox",
//                                                                                "Layers management toolbox",
//                                                                                QStringList()<<"layers");
//}


//void medViewLayersToolBox::update(dtkAbstractView * view)
//{
//    // Hide this toolbox when no views

//    if(!view)
//        this->hide();
//    else
//        this->show();

//    d->vtkView = qobject_cast<medVtkView*>(view);

//    if(d->vtkView)
//    {
//        addView(d->vtkView);

//        updateViews();

//        connect(d->vtkView, SIGNAL(dataAdded(dtkAbstractData*, int)), this, SLOT(updateViews()));
//        connect(d->vtkView, SIGNAL(dataRemoved(dtkAbstractData*, int)), this, SLOT(updateViews()));
//        connect(d->vtkView, SIGNAL(closed()), this ,SLOT(removeView()));
//        connect(d->vtkView, SIGNAL(closing()), this ,SLOT(removeView()));

//    }

//    d->viewParamsToolBox->update(view);
//}


//void medViewLayersToolBox::addView(medVtkView *view)
//{
//    if(!view)
//        return;

//    if( QApplication::keyboardModifiers() != Qt::ControlModifier )
//    {
//        d->viewList.clear();
//        d->viewMap.clear();
//    }

//    if(!d->viewList.contains(view))
//    {
//        d->viewList.append(view);

//        d->viewMap.insert(view->name(), view);
//    }
//}


//void medViewLayersToolBox::updateViews()
//{
//    updateLayerListWidget(d->viewList);

//    updateViewPool(d->viewList);

//    if( d->viewList.count() > 0 )
//    {
//      d->viewParamsToolBox->update(d->viewList[d->viewList.count()-1]);
//    }
//}


//void medViewLayersToolBox::updateLayerListWidget(QList<medVtkView*> vtkViews)
//{
//    resetList();

//    if (vtkViews.isEmpty()) {return;}

//    d->layersListWidget->blockSignals(true);

//    int lastLayer = -1;

//    foreach(medVtkView *view, vtkViews)
//    {
//        int nbLayers = view->layersCount();

//        for (int i  = 0; i < nbLayers; i++) {

//            medAbstractData *layerData = qobject_cast<medAbstractData*>(view->dataAtLayer(i));

//            if(layerData)
//            {
//                QString thumbPath = medMetaDataKeys::SeriesThumbnail.getFirstValue(layerData,":icons/layer.png");
//                QString layerName = medMetaDataKeys::SeriesDescription.getFirstValue(layerData,"<i>no name</i>");

//                QList<medAbstractVtkViewInteractor*> interactors;
//                foreach(dtkAbstractViewInteractor* i, view->interactors()) {
//                    medAbstractVtkViewInteractor * interactor = qobject_cast<medAbstractVtkViewInteractor*>(i);
//                    if ( interactor && interactor->isDataTypeHandled(layerData->identifier()))
//                        interactors.append(interactor);
//                }

//                QListWidgetItem * item = new QListWidgetItem(d->layersListWidget);
//                medLayerItemWidget * widget = new medLayerItemWidget(layerName, thumbPath, layerData, interactors, d->layersListWidget);
//                item->setSizeHint(widget->sizeHint());

//                d->layersListWidget->addItem(item);
//                d->layerItemHash.insert(item, QPair<QString, int>(view->name(), i));
//                d->layersListWidget->setItemWidget(item, widget);

//                lastLayer++;

//                connect(widget, SIGNAL(deletionRequested()), this, SLOT(removeLayer()));
//            }
//        }
//    }

//    d->layersListWidget->blockSignals(false);

//    if( lastLayer > -1 )
//    {
//        d->layersListWidget->clearSelection();
//        d->layersListWidget->item(lastLayer)->setSelected(true);
//    }
//}


//void medViewLayersToolBox::updateParameters()
//{
//    clearParams();

//    int nbLayers = d->layersListWidget->count();

//    QMultiMap<medVtkView*, int> layersSelecteds;

//    for (int i  = 0; i < nbLayers; i++)
//    {
//        QListWidgetItem * item = d->layersListWidget->item(i);
//        if( item->isSelected() )
//        {
//            QPair<QString, int> itemPair = d->layerItemHash.value(item);
//            QString viewName = itemPair.first;
//            int layer = itemPair.second;
//            layersSelecteds.insert(d->viewMap.value(viewName), layer);
//        }
//    }

//    updateParameters(layersSelecteds);
//}


//void medViewLayersToolBox::updateParameters(QMultiMap<medVtkView*, int> layersSelecteds)
//{
//    clearParams();

//    d->layerParamPool.clear();

//    QMap<QString, medAbstractParameter*> commonParamList;
//    commonParamList.setInsertInOrder(true);

//    QMapIterator<medVtkView*, int> i(layersSelecteds);

//    int j = 0;

//    while (i.hasNext())
//    {
//        i.next();

//        medVtkView* view =  i.key();
//        int layer = i.value();

//        medAbstractData * layerData = qobject_cast<medAbstractData*>(view->dataAtLayer(layer));

//        if(!layerData)
//            return;

//        foreach(dtkAbstractViewInteractor* i, view->interactors())
//        {
//            medAbstractVtkViewInteractor * interactor = qobject_cast<medAbstractVtkViewInteractor*>(i);
//            if ( interactor && interactor->isDataTypeHandled(layerData->identifier()))
//            {
//                QList<medAbstractParameter*> paramList = interactor->getParameters(view->dataAtLayer(layer));

//                foreach(medAbstractParameter *param, paramList)
//                {
//                    bool found = false;

//                    foreach(medAbstractParameter *existingParam, d->paramList)
//                    {
//                        if(param->match(existingParam))
//                        {
//                            found = true;
//                            d->layerParamPool.append(param);
//                            d->layerParamPool.append(existingParam);

//                            if(!commonParamList.contains(param->name()))
//                              commonParamList.insert(param->name(), param);

//                            break;
//                        }
//                    }
//                    if(!found)
//                    {
//                        d->paramList.append(param);
//                        d->paramMap.insert(param->name(), param);

//                        j++;
//                    }
//                }
//            }
//        }
//    }

//    QList<medAbstractParameter *> displayedParam;
//    if(layersSelecteds.count() == 1)
//        displayedParam = d->paramList;
//    else displayedParam = commonParamList.values();

//    foreach(medAbstractParameter *param, displayedParam)
//    {
//        QWidget *paramWidget = param->getWidget();
//        paramWidget->show();
//        d->paramWidgetList.append(paramWidget);
//        d->interactorsParamsLayout->addRow(param->name(),paramWidget);
//    }

//}


//void medViewLayersToolBox::updateViewPool(QList<medVtkView*> selectedViews)
//{
//    d->viewParamPool.clear();

//    if(selectedViews.size()>0)
//    {
//        foreach(medVtkView* view, selectedViews)
//        {
//            QList<medAbstractParameter*> paramList = d->viewParamsToolBox->getViewParameters(view);
//            foreach(medAbstractParameter *param, paramList)
//            {
//                d->viewParamPool.append(param);
//            }
//        }
//    }
//}


//void medViewLayersToolBox::resetList()
//{
//    d->layersListWidget->blockSignals(true);
//    d->layersListWidget->clear();
//    d->layersListWidget->blockSignals(false);

//    d->layerItemHash.clear();
//}


//void medViewLayersToolBox::clearParams()
//{
//   /* foreach(QWidget *widget, d->paramWidgetList)
//    {
//        widget->close();
//    }*/
//    d->paramWidgetList.clear();
//    d->paramList.clear();

//    QLayoutItem* item;
//    while ( ( item = d->interactorsParamsLayout->takeAt( 0 ) ) != NULL )
//    {
//        item->widget()->close();
//    }
//}


//void medViewLayersToolBox::removeView()
//{
//    medVtkView *sender = dynamic_cast<medVtkView*>(QObject::sender());
//    if(sender)
//    {
//        d->viewMap.remove(sender->name());
//        d->viewList.removeOne(sender);
//    }
//}


//void medViewLayersToolBox::removeLayer()
//{
//    medLayerItemWidget *sender = dynamic_cast<medLayerItemWidget*>(QObject::sender());

//    if(sender)
//    {
//        QList<QListWidgetItem*> items = d->layersListWidget->selectedItems();
//        foreach(QListWidgetItem* item, items)
//        {
//          d->layersListWidget->removeItemWidget(item);
//        }

//        d->vtkView->removeLayer(sender->dataItem());
//    }

//}


