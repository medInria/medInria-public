#include <medViewLayersToolBox.h>
#include <medToolBoxFactory.h>

#include <QWidgetList>

#include <medLayerItemWidget.h>
#include <medMetaDataKeys.h>
#include <medVtkView.h>
#include <medAbstractData.h>
#include <medAbstractVtkViewInteractor.h>

#include <QPushButton>

medViewLayersToolBox::medViewLayersToolBox(QWidget *parent)
    : medToolBox(parent)
    , vtkView(0)
{
    this->setTitle(tr("Layers management"));

    layersList = new QListWidget(this);
    layersList->setFocusPolicy(Qt::NoFocus);

    this->addWidget(layersList);
}


void medViewLayersToolBox::updateLayerList()
{
    resetList();
    if ( ! vtkView) {return;}

    int nbLayers = vtkView->numberOfLayers();

    qDebug() << "Layers:" << nbLayers;

    for (int i  = 0; i < nbLayers; i++) {

        medAbstractData * layerData = qobject_cast<medAbstractData*>(vtkView->layerData(i));

        QString thumbPath = medMetaDataKeys::SeriesThumbnail.getFirstValue(layerData,":icons/layer.png");
        QString layerName = medMetaDataKeys::SeriesDescription.getFirstValue(layerData,"<i>no name</i>");

        QList<medAbstractVtkViewInteractor*> interactors;
        foreach(dtkAbstractViewInteractor* i, vtkView->interactors()) {
            medAbstractVtkViewInteractor * interactor = qobject_cast<medAbstractVtkViewInteractor*>(i);
            if ( interactor && interactor->isDataTypeHandled(layerData->identifier()))
                interactors.append(interactor);
        }

        QListWidgetItem * item = new QListWidgetItem(layersList);
        medLayerItemWidget * widget = new medLayerItemWidget(layerName, thumbPath, layerData, interactors, layersList);
        item->setSizeHint(widget->sizeHint());

        layersList->addItem(item);
        layersList->setItemWidget(item, widget);
    }
}

void medViewLayersToolBox::resetList()
{
    layersList->clear();
}


// ------------------ Temporary code --------------------------------------


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

    if (vtkView)
        vtkView->disconnect(this);

    vtkView = qobject_cast<medVtkView*>(view);

    updateLayerList();
    if(vtkView) {
        connect(vtkView, SIGNAL(dataAdded(dtkAbstractData*, int)), this, SLOT(updateLayerList()));
        connect(vtkView, SIGNAL(dataRemoved(dtkAbstractData*, int)), this, SLOT(updateLayerList()));
    }
}
