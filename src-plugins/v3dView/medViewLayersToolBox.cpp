#include <medViewLayersToolBox.h>
#include <medToolBoxFactory.h>

#include <QWidgetList>

#include <medLayerItemWidget.h>
#include <medMetaDataKeys.h>
#include <medVtkView.h>
#include <medAbstractData.h>
#include <medAbstractVtkViewInteractor.h>
#include <medParameter.h>
#include <medViewParamsToolBox.h>
#include <medToolBoxHeader.h>
#include <medViewContainer.h>

#include <QPushButton>

class medViewLayersToolBoxPrivate
{
public:

    QListWidget * viewListWidget;
    QListWidget * layersListWidget;
    QVBoxLayout * interactorsParamsLayout;
    medVtkView  * vtkView;
    QList<QWidget*> paramWidgetList;
    QList<medVtkView*> viewList;
    QMap<QString,medVtkView*> viewMap;
    QHash<QListWidgetItem*, QPair<QString,int> > layerItemHash;

    QList<medAbstractParameter*> paramList;
    QMap<QString, medAbstractParameter*> paramMap;

    medParameterPool layerParamPool;
    medParameterPool viewParamPool;
    medViewParamsToolBox *viewParamsToolBox;
};




medViewLayersToolBox::medViewLayersToolBox(QWidget *parent)
    : medToolBox(parent), d(new medViewLayersToolBoxPrivate)
{
    this->setTitle(tr("Layers management"));

    d->vtkView = 0;

    d->viewListWidget = new QListWidget(this);
    d->viewListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->viewListWidget->setAlternatingRowColors(true);

    d->layersListWidget = new QListWidget(this);
    d->layersListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QWidget *interactorsParamsWidget = new QWidget(this);
    d->interactorsParamsLayout = new QVBoxLayout(interactorsParamsWidget);

    d->viewParamsToolBox = new medViewParamsToolBox();
    d->viewParamsToolBox->header()->hide();

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);

    this->addWidget(d->viewListWidget);
    this->addWidget(line);
    this->addWidget(d->layersListWidget);
    this->addWidget(d->viewParamsToolBox);
    this->addWidget(interactorsParamsWidget);

    d->paramWidgetList = QList<QWidget*>();
    d->viewMap = QMap<QString, medVtkView*>();

    connect(d->viewListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateViews()));
    connect(d->layersListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updateParameters()));
}




bool medViewLayersToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medViewLayersToolBox>("medViewLayersToolBox","medViewLayersToolBox",
                                                                                "Layers management toolbox",
                                                                                QStringList()<<"view"<<"layers");
}


void medViewLayersToolBox::update(dtkAbstractView * view)
{
    // Hide this toolbox when no views

    if(!view)
        this->hide();
    else
        this->show();

    d->vtkView = qobject_cast<medVtkView*>(view);

    if(d->vtkView)
    {
        addView(d->vtkView);
        selectView(d->vtkView);

        connect(d->vtkView, SIGNAL(dataAdded(dtkAbstractData*, int)), this, SLOT(updateViews()));
        connect(d->vtkView, SIGNAL(dataRemoved(dtkAbstractData*, int)), this, SLOT(updateViews()));
        connect(d->vtkView, SIGNAL(closed()), this ,SLOT(removeView()));
        connect(d->vtkView, SIGNAL(closing()), this ,SLOT(removeView()));
    }

    d->viewParamsToolBox->update(view);
}


void medViewLayersToolBox::addView(medVtkView *view)
{
    if(!view)
        return;

    if(!d->viewList.contains(view))
    {
        d->viewList.append(view);

        d->viewMap.insert(view->name(), view);

        updateViewListWidget();
    }
}


void medViewLayersToolBox::updateViews()
{
    QList<medVtkView*> selectedViews;

    for (int i  = 0; i < d->viewList.count(); i++)
    {
        if( d->viewListWidget->item(i)->isSelected() )
        {
            selectedViews.append(d->viewList.at(i));
        }
    }

    updateLayerListWidget(selectedViews);

    updateViewPool(selectedViews);

    if( selectedViews.count() > 0 )
    {
      d->viewParamsToolBox->update(selectedViews[selectedViews.count()-1]);
    }

}

void medViewLayersToolBox::updateViewListWidget()
{
    d->viewListWidget->blockSignals(true);
    d->viewListWidget->clear();
    d->viewListWidget->blockSignals(false);

    foreach(medVtkView *vtkView, d->viewList)
    {
        QListWidgetItem * item = new QListWidgetItem(vtkView->name(), d->viewListWidget);
        d->viewListWidget->addItem(item);
    }
}


void medViewLayersToolBox::selectView(medVtkView *view)
{
    if(!view)
        return;
    d->viewListWidget->blockSignals(true);

    if( !(QApplication::keyboardModifiers() & Qt::ControlModifier) )
    {
        d->viewListWidget->clearSelection();
    }

    for (int i  = 0; i < d->viewList.count(); i++)
    {
        if( d->viewListWidget->item(i)->text() == view->name() )
        {
            d->viewListWidget->item(i)->setSelected(true);
        }
    }

    d->viewListWidget->blockSignals(false);
}


void medViewLayersToolBox::updateLayerListWidget(QList<medVtkView*> vtkViews)
{
    resetList();

    if (vtkViews.isEmpty()) {return;}

    foreach(medVtkView *view, vtkViews)
    {
        medViewContainer *container = dynamic_cast<medViewContainer*>(view->parent());
        //if(!container->isSelected())
          container->select();

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
            }
        }
    }
}


void medViewLayersToolBox::updateParameters()
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

    updateParameters(selectedLayers);
}


void medViewLayersToolBox::updateParameters(QMultiMap<medVtkView*, int> selectedLayers)
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

                    foreach(medAbstractParameter *existingParam, d->paramList)
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
                        d->paramList.append(param);
                        d->paramMap.insert(param->name(), param);

                        j++;
                    }
                }
            }
        }
    }

    QList<medAbstractParameter *> displayedParam;
    if(selectedLayers.count() == 1)
        displayedParam = d->paramList;
    else displayedParam = commonParamList.values();

    foreach(medAbstractParameter *param, displayedParam)
    {
        QWidget *paramWidget = param->getWidget();
        paramWidget->show();
        d->paramWidgetList.append(paramWidget);
        d->interactorsParamsLayout->addWidget(paramWidget);
    }

}


void medViewLayersToolBox::updateViewPool(QList<medVtkView*> selectedViews)
{
    d->viewParamPool.clear();

    if(selectedViews.size()>0)
    {
        foreach(medVtkView* view, selectedViews)
        {
            QList<medAbstractParameter*> paramList = d->viewParamsToolBox->getParameters(view);
            foreach(medAbstractParameter *param, paramList)
            {
                d->viewParamPool.append(param);
            }
        }
    }
}


void medViewLayersToolBox::resetList()
{
    d->layersListWidget->clear();
    d->layerItemHash.clear();
}


void medViewLayersToolBox::clearParams()
{
    foreach(QWidget *widget, d->paramWidgetList)
    {
        widget->close();
    }
    d->paramWidgetList.clear();
    d->paramList.clear();

}


void medViewLayersToolBox::removeView()
{
    medVtkView *sender = dynamic_cast<medVtkView*>(QObject::sender());
    if(sender)
    {
        d->viewMap.remove(sender->name());
        d->viewList.removeOne(sender);
    }

    updateViewListWidget();
}


