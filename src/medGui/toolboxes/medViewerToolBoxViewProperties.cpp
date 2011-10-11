/*
* medViewerToolBoxViewProperties.cpp
*
*  Created on: 15 févr. 2011
*      Author: aabadie
*/

#include <medViewerToolBoxViewProperties.h>

#include <dtkCore/dtkAbstractData.h>

#include <medStorage.h>
#include <medMetaDataKeys.h>
#include <medAbstractView.h>
#include <medToolBoxTab.h>
#include <medMeshAbstractViewInteractor.h>

class medViewerToolBoxViewPropertiesPrivate
{
public:

    QTreeWidget * propertiesTree;
    QWidget * propertiesView;
    QWidget * propView;
    QStringList lutList;
    QStringList attributeList;
    QStringList presetList;
    QStringList renderingList;
    int currentLayer;
    QWidget * twoLayersWidget;
    QSlider * slider;
    QString thumbnailLocation;
    QString textLayer0;
    QString textLayer1;
    QPushButton * switchLayersButton;
    QList<medMeshAbstractViewInteractor*> interactors;
    medAbstractView *view;

    QPushButton *windowingPushButton;
    QPushButton *zoomingPushButton;
    QPushButton *slicingPushButton;
    QPushButton *measuringPushButton;

    QCheckBox *scalarBarVisibilityCheckBox;
    QCheckBox *axisVisibilityCheckBox;
    QCheckBox *rulerVisibilityCheckBox;
    QCheckBox *annotationsVisibilityCheckBox;

    QComboBox * view3dModeComboBox;
    QComboBox * view3dVRModeComboBox;
    QSlider * view3dLODSlider;
    QPushButton * croppingPushButton;
    QWidget * view3dToolBoxWidget;

    QString thumbLocation;

    QTreeWidgetItem * layerItem;
    bool isMesh;
    int currentInteractor;

    QList <QComboBox *> meshLutBoxList;
};

medViewerToolBoxViewProperties::medViewerToolBoxViewProperties(QWidget *parent) :
medToolBox(parent), d(new medViewerToolBoxViewPropertiesPrivate)
{

    d->view = 0;
    d->currentLayer = 0;
    d->textLayer0 = tr("Switch to layer 0 only");
    d->textLayer1 = tr("Switch to layer 1 only");
    d->isMesh = false;
    d->currentInteractor = 0;
    d->lutList << "Default" << "Black & White" << "Black & White Inversed" << "Spectrum" << "Hot Metal" << "Hot Green"
        << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni 2" << "Asymmetry" << "P-Value" << "Red Black Alpha"
        << "Green Black Alpha" << "Blue Black Alpha" << "Muscles & Bones" << "Bones" << "Red Vessels"
        << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body";
    d->presetList << "None" << "VR Muscles&Bones" << "Vascular I" << "Vascular II" << "Vascular III" << "Vascular IV"
        << "Standard" << "Soft" << "Soft on White" << "Soft on Blue" << "Red on White" << "Glossy" ;
    d->renderingList << "wireframe"<<"surface"<<"points";
    d->attributeList << "Solid";
    //lutBox = new QComboBox();

    d->propertiesTree = new QTreeWidget(this);
    d->propertiesTree->setFocusPolicy(Qt::NoFocus);

    d->propertiesTree->setColumnCount(3);
    d->propertiesTree->setColumnWidth(0,50);
    d->propertiesTree->setSelectionMode(QAbstractItemView::NoSelection);
    QStringList headers;
    headers << "Object" << "Name" << "Value";
    d->propertiesTree->setHeaderLabels(headers);
    d->propertiesTree->setAnimated(true);
    d->propertiesTree->setAlternatingRowColors(true);
    d->propertiesTree->setRootIsDecorated(false);
    d->propertiesTree->setContextMenuPolicy(Qt::CustomContextMenu);

    this->setTitle("View Properties");

    QObject::connect(d->propertiesTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onItemClicked(QTreeWidgetItem *, int)));
    QObject::connect(d->propertiesTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextTreeMenu(QPoint)));

    d->propertiesView = new QWidget;

    d->windowingPushButton = new QPushButton("", this);
    d->windowingPushButton->setFocusPolicy(Qt::NoFocus);
    d->windowingPushButton->setIcon (QIcon (":/icons/wlww.png"));
    d->windowingPushButton->setCheckable (true);
    d->windowingPushButton->setMinimumWidth ( 20 );
    d->zoomingPushButton   = new QPushButton("", this);
    d->zoomingPushButton->setFocusPolicy(Qt::NoFocus);
    d->zoomingPushButton->setIcon (QIcon (":/icons/magnify.png"));
    d->zoomingPushButton->setCheckable (true);
    d->slicingPushButton   = new QPushButton("", this);
    d->slicingPushButton->setFocusPolicy(Qt::NoFocus);
    d->slicingPushButton->setIcon (QIcon (":/icons/stack.png"));
    d->slicingPushButton->setCheckable (true);
    d->measuringPushButton = new QPushButton("", this);
    d->measuringPushButton->setFocusPolicy(Qt::NoFocus);
    d->measuringPushButton->setIcon (QIcon (":/icons/length.png"));
    d->measuringPushButton->setCheckable (true);

    QButtonGroup *mouseGroup = new QButtonGroup (this);
    mouseGroup->addButton ( d->windowingPushButton );
    mouseGroup->addButton ( d->zoomingPushButton );
    mouseGroup->addButton ( d->slicingPushButton );
    mouseGroup->addButton ( d->measuringPushButton );
    mouseGroup->setExclusive (true);

    d->propView = new QWidget;
    QHBoxLayout * propLayout = new QHBoxLayout;

    d->scalarBarVisibilityCheckBox = new QCheckBox();
    d->scalarBarVisibilityCheckBox->setFocusPolicy(Qt::NoFocus);
    d->axisVisibilityCheckBox = new QCheckBox();
    d->axisVisibilityCheckBox->setFocusPolicy(Qt::NoFocus);
    d->rulerVisibilityCheckBox = new QCheckBox();
    d->rulerVisibilityCheckBox->setFocusPolicy(Qt::NoFocus);
    d->annotationsVisibilityCheckBox = new QCheckBox();
    d->annotationsVisibilityCheckBox->setFocusPolicy(Qt::NoFocus);

    propLayout->addWidget(d->scalarBarVisibilityCheckBox);
    propLayout->addWidget(d->annotationsVisibilityCheckBox);
    d->propView->setLayout(propLayout);
    propLayout->setAlignment(Qt::AlignJustify);
    d->scalarBarVisibilityCheckBox->setText(tr("Scalar Bar"));
    d->axisVisibilityCheckBox->setText(tr("Axis"));
    d->rulerVisibilityCheckBox->setText(tr("Ruler"));
    d->annotationsVisibilityCheckBox->setText("Annotations");
    d->annotationsVisibilityCheckBox->setChecked(true);
    d->rulerVisibilityCheckBox->setChecked(true);

    QHBoxLayout *mouseLayout = new QHBoxLayout;
    mouseLayout->addWidget(d->axisVisibilityCheckBox);
    mouseLayout->addWidget(d->windowingPushButton);
    mouseLayout->addWidget(d->slicingPushButton);
    mouseLayout->addWidget(d->zoomingPushButton);
    mouseLayout->addWidget(d->measuringPushButton);
    mouseLayout->addWidget(d->rulerVisibilityCheckBox);

    QFormLayout * propertiesViewLayout = new QFormLayout(d->propertiesView);
    propertiesViewLayout->addRow (/*"Type:", */mouseLayout);
    //propertiesViewLayout->addRow (d->scalarBarVisibilityCheckBox);
    propertiesViewLayout->setFormAlignment(Qt::AlignHCenter);

    connect(d->windowingPushButton,           SIGNAL(toggled(bool)),                this, SLOT(onWindowingChanged(bool)));
    connect(d->zoomingPushButton,             SIGNAL(toggled(bool)),                this, SLOT(onZoomingChanged(bool)));
    connect(d->slicingPushButton,             SIGNAL(toggled(bool)),                this, SLOT(onSlicingChanged(bool)));
    connect(d->measuringPushButton,           SIGNAL(toggled(bool)),                this, SLOT(onMeasuringChanged(bool)));

    d->view3dModeComboBox = new QComboBox(this);
    d->view3dModeComboBox->setFocusPolicy(Qt::NoFocus);
    d->view3dModeComboBox->addItem("VR");
    d->view3dModeComboBox->addItem("MIP - Maximum");
    d->view3dModeComboBox->addItem("MIP - Minimum");
    d->view3dModeComboBox->addItem("MPR");
    d->view3dModeComboBox->addItem("Off");


    d->view3dVRModeComboBox = new QComboBox(this);
    d->view3dVRModeComboBox->setFocusPolicy(Qt::NoFocus);
    d->view3dVRModeComboBox->addItem( "GPU" );
    d->view3dVRModeComboBox->addItem( "Ray Cast / Texture" );
    d->view3dVRModeComboBox->addItem( "Ray Cast" );
    d->view3dVRModeComboBox->addItem( "Texture" );
    d->view3dVRModeComboBox->addItem( "Default" );


    d->view3dLODSlider = new QSlider (Qt::Horizontal, this);
    d->view3dLODSlider->setFocusPolicy(Qt::NoFocus);
    d->view3dLODSlider->setRange (0, 100);
    d->view3dLODSlider->setValue (100);
    d->view3dLODSlider->setTracking( false );

    d->croppingPushButton = new QPushButton ("", this);
    d->croppingPushButton->setFocusPolicy(Qt::NoFocus);
    d->croppingPushButton->setIcon (QIcon (":/icons/cropping.png"));
    d->croppingPushButton->setCheckable (true);
    d->croppingPushButton->setMinimumWidth ( 20 );

    connect(d->view3dModeComboBox,            SIGNAL(currentIndexChanged(QString)), this, SLOT(onModeChanged(QString)));
    connect(d->view3dVRModeComboBox,          SIGNAL(currentIndexChanged(QString)), this, SLOT(onVRModeChanged(QString)));
    connect(d->view3dLODSlider,               SIGNAL(valueChanged(int)),            this, SLOT(onLodChanged(int)));
    connect(d->croppingPushButton,            SIGNAL(toggled(bool)),                this, SLOT(onCroppingChanged(bool)));

    d->view3dToolBoxWidget = new QWidget;
    QFormLayout *view3dToolBoxWidgetLayout = new QFormLayout(d->view3dToolBoxWidget);
    view3dToolBoxWidgetLayout->addRow(tr("3D Mode"), d->view3dModeComboBox);
    view3dToolBoxWidgetLayout->addRow(tr("Renderer:"), d->view3dVRModeComboBox);
    view3dToolBoxWidgetLayout->addRow(tr("LOD:"), d->view3dLODSlider);
    view3dToolBoxWidgetLayout->addRow(tr("Cropping:"), d->croppingPushButton);
    view3dToolBoxWidgetLayout->setFormAlignment(Qt::AlignHCenter);



    //add 2 layers opacity slider and switcher.
    d->twoLayersWidget = new QWidget(this);
    QGridLayout * twoLayersLayout = new QGridLayout();
    d->twoLayersWidget->setLayout(twoLayersLayout);

    d->slider = new QSlider(Qt::Horizontal,this);
    d->slider->setFocusPolicy(Qt::NoFocus);
    d->slider->setRange(0,100);
    d->slider->setValue(50);
    QObject::connect(d->slider, SIGNAL(valueChanged(int)), this, SLOT(on2LayersOpacitySliderSet(int)));
    twoLayersLayout->addWidget(new QLabel(tr("Layers' opacity:"),this),
        0,0,1,3,Qt::AlignLeft);
    twoLayersLayout->addWidget(new QLabel(tr("Layer 0"),this),1,0,1,1);
    twoLayersLayout->addWidget(d->slider,1,1,1,1);
    twoLayersLayout->addWidget(new QLabel(tr("Layer 1"),this),1,2,1,1);
    d->switchLayersButton = new QPushButton(d->textLayer0,this);
    twoLayersLayout->addWidget(d->switchLayersButton,2,0,1,3);

    QObject::connect(d->switchLayersButton,SIGNAL(clicked()),
        this,SLOT(onSwitchLayersButtonClicked()));

    connect(d->scalarBarVisibilityCheckBox, SIGNAL(toggled(bool)), this, SLOT(onScalarBarVisibilityChanged(bool)));
    connect(d->axisVisibilityCheckBox, SIGNAL(toggled(bool)), this, SLOT(onAxisVisibilityChanged(bool)));
    connect(d->rulerVisibilityCheckBox, SIGNAL(toggled(bool)), this, SLOT(onRulerVisibilityChanged(bool)));
    connect(d->annotationsVisibilityCheckBox, SIGNAL(toggled(bool)), this, SLOT(onAnnotationsVisibilityChanged(bool)));





    this->hide();

    this->addWidget(d->propertiesView);
    this->addWidget(d->view3dToolBoxWidget);
    d->view3dToolBoxWidget->hide();
    this->addWidget(d->propertiesTree);
    this->addWidget(d->twoLayersWidget);
    d->twoLayersWidget->hide();
    this->addWidget(d->propView);
}

medViewerToolBoxViewProperties::~medViewerToolBoxViewProperties(void)
{
    delete d;
    d = NULL;
}

void
    medViewerToolBoxViewProperties::update(dtkAbstractView *view)
{

    medToolBox::update(view);
    if(!view)
    {
        clear();
        return;
    }

    if ((d->view) && (d->view != dynamic_cast<medAbstractView *> (view)) )
    {
        d->view->disconnect(this,0);
        clear();
    }

//    qDebug() << "update 1";
    if (medAbstractView *medView = dynamic_cast<medAbstractView *> (view))
    {

        if ((d->view == dynamic_cast<medAbstractView *> (view)))
        {
            return;
        }
        d->view = medView;
        d->propertiesTree->clear();
        //decide whether to show the 2 layers slider
        raiseSlider(d->view->layerCount() == 2);

//        qDebug() << "update 2";
        if(d->view->meshLayerCount()!=0)
            if (medMeshAbstractViewInteractor *interactor = dynamic_cast<medMeshAbstractViewInteractor*>(d->view->interactor ("v3dViewMeshInteractor")))
            {
                d->currentInteractor = d->interactors.indexOf(interactor);
            }

//        qDebug() << "update 3";

        for (int i = 0, meshNumber = 0, imageNumber = 0; i < d->view->layerCount() + d->view->meshLayerCount(); i++)
        {
            if(d->view->dataInList(i) && d->view->dataInList(i)->description().contains("vtkDataMesh"))
            {
                this->constructMeshLayer(d->view->dataInList(i), meshNumber);
                meshNumber++;
            }
            else
            {
//                qDebug() << "update 4" << imageNumber;
                this->constructImageLayer(d->view->dataInList(i), imageNumber);
                imageNumber++;
            }

            d->propertiesTree->collapseAll();
        }

        QObject::connect(d->view, SIGNAL(dataAdded(dtkAbstractData*, int)),
                         this, SLOT(onDataAdded(dtkAbstractData*, int)),
                         Qt::UniqueConnection);
//        QObject::connect(d->view, SIGNAL(closing()), this, SLOT(onViewClosed()), Qt::UniqueConnection);

        QObject::connect(d->view, SIGNAL(TwoDTriggered(dtkAbstractView*)),
                         this, SLOT(on2DTriggered(dtkAbstractView*)),
                         Qt::UniqueConnection);
        QObject::connect(d->view, SIGNAL(ThreeDTriggered(dtkAbstractView*)),
                         this, SLOT(on3DTriggered(dtkAbstractView*)),
                         Qt::UniqueConnection);
        this->on2DTriggered(d->view);
        this->on3DTriggered(d->view);

        d->view3dModeComboBox->blockSignals(true);
        d->view3dModeComboBox->setCurrentIndex(d->view3dModeComboBox->findText(view->property("3DMode")));
        d->view3dModeComboBox->blockSignals(false);
        //qDebug() << "update 5";
    }
}

void medViewerToolBoxViewProperties::constructImageLayer(dtkAbstractData* data, int imageLayer)
{
    if(!data)
        return;
    QString layerItemString = QString::number(imageLayer);
    d->thumbLocation = ":icons/layer.png";

    //Check the layer does not already exists, in which case we remove it.
    foreach (QTreeWidgetItem* item,
             d->propertiesTree->findItems(layerItemString,
                                          Qt::MatchExactly | Qt::MatchWrap,0))
    {
//        qDebug() << "Found item" << item->text(0);
        int index = d->propertiesTree->indexOfTopLevelItem(item);
        d->propertiesTree->takeTopLevelItem(index);
    }

    if (data)
    {
        if (medMetaDataKeys::SeriesThumbnail.is_set_in(data))
        {
            d->thumbLocation = medMetaDataKeys::SeriesThumbnail.getFirstValue(data,":icons/layer.png");
        }
    }

    d->layerItem = new QTreeWidgetItem(d->propertiesTree->invisibleRootItem(), QTreeWidgetItem::UserType+1);
    d->layerItem->setText(0, QString::number(imageLayer));
    d->layerItem->setIcon(0,QIcon(d->thumbLocation));
    if (data!= NULL && medMetaDataKeys::SeriesDescription.is_set_in(data))
    {
        d->layerItem->setToolTip(0,data->metaDataValues(medMetaDataKeys::PatientName.key())[0]
        + "\n" + data->metaDataValues(medMetaDataKeys::StudyDescription.key())[0]
        + "\n" + data->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0]);
    }

    QTreeWidgetItem * visibleItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    visibleItem->setText(1, "Visible");
    QCheckBox * visibleBox = new QCheckBox();
    visibleBox->setChecked(d->view->visibility(imageLayer));
    d->propertiesTree->setItemWidget(visibleItem, 2, visibleBox);


    QTreeWidgetItem * opacityItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    opacityItem->setText(1, "Opacity");
    QSlider * opacityBox = new QSlider(Qt::Horizontal);
    opacityBox->setRange(0,100);
    opacityBox->setValue(d->view->opacity(imageLayer) * 100);
    d->propertiesTree->setItemWidget(opacityItem, 2, opacityBox);


    QTreeWidgetItem * lutItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    lutItem->setText(1, "LUT");
    QComboBox * lutBox = new QComboBox();
    lutBox->setFocusPolicy(Qt::NoFocus);
    lutBox->addItem("Default");
    lutBox->addItem("Black & White");
    lutBox->addItem("Black & White Inversed");
    lutBox->addItem("Spectrum");
    lutBox->addItem("Hot Metal");
    lutBox->addItem("Hot Green");
    lutBox->addItem("Hot Iron");
    lutBox->addItem("GE");
    lutBox->addItem("Flow");
    lutBox->addItem("Loni");
    lutBox->addItem("Loni 2");
    lutBox->addItem("Asymmetry");
    lutBox->addItem("P-Value");
    lutBox->addItem("Red Black Alpha");
    lutBox->addItem("Green Black Alpha");
    lutBox->addItem("Blue Black Alpha");
    lutBox->addItem("Muscles & Bones");
    lutBox->addItem("Bones");
    lutBox->addItem("Red Vessels");
    lutBox->addItem("Cardiac");
    lutBox->addItem("Gray Rainbow");
    lutBox->addItem("Stern");
    lutBox->addItem("Black Body");

    d->propertiesTree->setItemWidget(lutItem, 2, lutBox);
    lutBox->blockSignals(true);
    lutBox->setCurrentIndex(lutBox->findText(d->view->getLUT(imageLayer)));
    lutBox->blockSignals(false);



    QTreeWidgetItem * presetItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    presetItem->setText(1, "Preset");
    QComboBox * presetBox = new QComboBox();
    presetBox->setFocusPolicy(Qt::NoFocus);
    presetBox->addItem("None");
    presetBox->addItem("VR Muscles&Bones");
    presetBox->addItem("Vascular I");
    presetBox->addItem("Vascular II");
    presetBox->addItem("Vascular III");
    presetBox->addItem("Vascular IV");
    presetBox->addItem("Standard");
    presetBox->addItem("Soft");
    presetBox->addItem("Soft on White");
    presetBox->addItem("Soft on Blue");
    presetBox->addItem("Red on White");
    presetBox->addItem("Glossy");

    d->propertiesTree->setItemWidget(presetItem, 2, presetBox);
    presetBox->blockSignals(true);
    presetBox->setCurrentIndex(presetBox->findText(d->view->getPreset(imageLayer)));
    presetBox->blockSignals(false);

    QObject::connect(visibleBox, SIGNAL(stateChanged(int)), this, SLOT(onVisibilitySet(int)));
    QObject::connect(opacityBox, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySliderSet(int)));
    QObject::connect(lutBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLUTChanged(int)));
    QObject::connect(presetBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPresetChanged(int)));

    d->propertiesTree->collapseAll();

}
void medViewerToolBoxViewProperties::constructMeshLayer(dtkAbstractData* data, int meshLayer)
{
    if(!data)
        return;
   // QString layerItemString;
   // if(d->view->layerCount() > 1 || d->view->meshLayerCount() > 0 )
    //    if(data)
    QString layerItemString = "Mesh " + QString::number(meshLayer);

    //Check the layer does not already exists, in which case we remove it.
    foreach (QTreeWidgetItem* item,
             d->propertiesTree->findItems(layerItemString,
                                          Qt::MatchExactly | Qt::MatchWrap,0))
    {
//        qDebug() << "Found item" << item->text(0);
        int index = d->propertiesTree->indexOfTopLevelItem(item);
        d->propertiesTree->takeTopLevelItem(index);
    }

    d->thumbLocation = ":icons/layer.png";


    if (data)
        if (medMetaDataKeys::SeriesThumbnail.is_set_in(data))
            d->thumbLocation = medMetaDataKeys::SeriesThumbnail.getFirstValue(data,":icons/layer.png");

    d->layerItem = new QTreeWidgetItem(d->propertiesTree->invisibleRootItem(), QTreeWidgetItem::UserType+1);
    d->layerItem->setText(0, layerItemString);
    d->layerItem->setIcon(0,QIcon(d->thumbLocation));
    if (medMetaDataKeys::SeriesDescription.is_set_in(data)) {
        d->layerItem->setToolTip(0,data->metaDataValues(medMetaDataKeys::PatientName.key())[0]
        + "\n" + data->metaDataValues(medMetaDataKeys::StudyDescription.key())[0]
        + "\n" + data->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0]);
    }


    QTreeWidgetItem * meshVisibleItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    meshVisibleItem->setText(1, "Visible");
    QCheckBox * meshVisibleBox = new QCheckBox();
    meshVisibleBox->setChecked(d->interactors[d->currentInteractor]->visibility(meshLayer));
    d->propertiesTree->setItemWidget(meshVisibleItem, 2, meshVisibleBox);


    QTreeWidgetItem * opacityItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    opacityItem->setText(1, "Opacity");
    QSlider * opacityBox = new QSlider(Qt::Horizontal);
    opacityBox->setRange(0,100);
    opacityBox->setValue(d->interactors[d->currentInteractor]->opacity(meshLayer) * 100);
    d->propertiesTree->setItemWidget(opacityItem, 2, opacityBox);


    QTreeWidgetItem * attrMap = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    attrMap->setText(1, "Attributes");
    QComboBox * attrBox = new QComboBox();
    attrBox->setFocusPolicy(Qt::NoFocus);
    attrBox->addItem("Solid");
    if (d->interactors[d->currentInteractor]->getLUTQuery(meshLayer)!=NULL)
    {
        attrBox->addItem(d->interactors[d->currentInteractor]->getLUTQuery(meshLayer));
        d->attributeList << d->interactors[d->currentInteractor]->getLUTQuery(meshLayer);
    }
    d->propertiesTree->setItemWidget(attrMap, 2, attrBox);
    attrBox->setCurrentIndex(attrBox->findText(*(d->interactors[d->currentInteractor]->attribute(meshLayer))));

    QTreeWidgetItem * lutItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    lutItem->setText(1, "LUT");
    QComboBox * lutBox = new QComboBox();

    if(!d->meshLutBoxList.at(meshLayer) ){
    d->meshLutBoxList.append(lutBox);

    }
    else
    d->meshLutBoxList.replace(meshLayer, lutBox);

    lutBox->setFocusPolicy(Qt::NoFocus);

    if(attrBox->currentIndex()!=0)
    {
    lutBox->addItem("Default");
    lutBox->addItem("Black & White");
    lutBox->addItem("Black & White Inversed");
    lutBox->addItem("Spectrum");
    lutBox->addItem("Hot Metal");
    lutBox->addItem("Hot Green");
    lutBox->addItem("Hot Iron");
    lutBox->addItem("GE");
    lutBox->addItem("Flow");
    lutBox->addItem("Loni");
    lutBox->addItem("Loni 2");
    lutBox->addItem("Asymmetry");
    lutBox->addItem("P-Value");
    lutBox->addItem("Red Black Alpha");
    lutBox->addItem("Green Black Alpha");
    lutBox->addItem("Blue Black Alpha");
    lutBox->addItem("Muscles & Bones");
    lutBox->addItem("Bones");
    lutBox->addItem("Red Vessels");
    lutBox->addItem("Cardiac");
    lutBox->addItem("Gray Rainbow");
    lutBox->addItem("Stern");
    lutBox->addItem("Black Body");
    }
    d->propertiesTree->setItemWidget(lutItem, 2, lutBox);
    lutBox->blockSignals(true);
    lutBox->setCurrentIndex(lutBox->findText(*(d->interactors[d->currentInteractor]->lut(meshLayer))));
    lutBox->blockSignals(false);


    QTreeWidgetItem * edgeVisibleItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    edgeVisibleItem->setText(1, "Edge Visible");
    QCheckBox * edgeVisibleBox = new QCheckBox();
    edgeVisibleBox->setChecked(d->interactors[d->currentInteractor]->edgeVisibility(meshLayer));
    d->propertiesTree->setItemWidget(edgeVisibleItem, 2, edgeVisibleBox);


    QTreeWidgetItem * coloringItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    coloringItem->setText(1, "Color");
    QComboBox *colorComboBox = new QComboBox();
    colorComboBox->addItem("Default");
    colorComboBox->addItem(createIcon("#000000"),"#000000",QColor("#000000"));
    colorComboBox->addItem(createIcon("#FFFFFF"),"#FFFFFF",QColor("#FFFFFF"));
    colorComboBox->addItem(createIcon("#808080"),"#808080",QColor("#808080"));
    colorComboBox->addItem(createIcon("#800000"),"#800000",QColor("#800000"));
    colorComboBox->addItem(createIcon("#804040"),"#804040",QColor("#804040"));
    colorComboBox->addItem(createIcon("#FF8080"),"#FF8080",QColor("#FF8080"));
    colorComboBox->addItem(createIcon("#FF0000"),"#FF0000",QColor("#FF0000"));
    colorComboBox->addItem(createIcon("#FFFF80"),"#FFFF80",QColor("#FFFF80"));
    colorComboBox->addItem(createIcon("#FFFF00"),"#FFFF00",QColor("#FFFF00"));
    colorComboBox->addItem(createIcon("#FF8040"),"#FF8040",QColor("#FF8040"));
    colorComboBox->addItem(createIcon("#FF8000"),"#FF8000",QColor("#FF8000"));
    colorComboBox->addItem(createIcon("#80FF80"),"#80FF80",QColor("#80FF80"));
    colorComboBox->addItem(createIcon("#80FF00"),"#80FF00",QColor("#80FF00"));
    colorComboBox->addItem(createIcon("#00FF00"),"#00FF00",QColor("#00FF00"));
    colorComboBox->addItem(createIcon("#80FFFF"),"#80FFFF",QColor("#80FFFF"));
    colorComboBox->addItem(createIcon("#00FFFF"),"#00FFFF",QColor("#00FFFF"));
    colorComboBox->addItem(createIcon("#004080"),"#004080",QColor("#004080"));
    colorComboBox->addItem(createIcon("#0000FF"),"#0000FF",QColor("#0000FF"));
    colorComboBox->addItem(createIcon("#0080FF"),"#0080FF",QColor("#0080FF"));
    colorComboBox->addItem(createIcon("#0080C0"),"#0080C0",QColor("#0080C0"));
    d->propertiesTree->setItemWidget(coloringItem, 2, colorComboBox);

    colorComboBox->setCurrentIndex(colorComboBox->findText(*(d->interactors[d->currentInteractor]->color(meshLayer))));

    QTreeWidgetItem * renderingItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    renderingItem->setText(1, "Rendering");
    QComboBox * renderingBox = new QComboBox();
    renderingBox->setFocusPolicy(Qt::NoFocus);
    //renderingBox->addItem("Default");
    renderingBox->addItem("Wire frame");
    renderingBox->addItem("Surface");
    renderingBox->addItem("Points");

    d->propertiesTree->setItemWidget(renderingItem, 2, renderingBox);
    renderingBox->setCurrentIndex(renderingBox->findText(*(d->interactors[d->currentInteractor]->renderingType(meshLayer))));

    QObject::connect(meshVisibleBox, SIGNAL(stateChanged(int)), this, SLOT(onVisibilitySet(int)));
    QObject::connect(opacityBox, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySliderSet(int)));
    QObject::connect(attrBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAttrBoxChanged(int)));
    QObject::connect(lutBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLUTChanged(int)));
    QObject::connect(edgeVisibleBox, SIGNAL(stateChanged(int)), this, SLOT(onEdgeVisibilitySet(int)));
    QObject::connect(colorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onColorChanged(int)));
    QObject::connect(renderingBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRenderingChanged(int)));
    d->propertiesTree->collapseAll();


}
void
    medViewerToolBoxViewProperties::onDataAdded(dtkAbstractData* data)
{

}
void medViewerToolBoxViewProperties::onDataAdded( int layer)
{

}
void
    medViewerToolBoxViewProperties::onDataAdded(dtkAbstractData* data, int layer)
{
    if(!data)
        return;
    if (!d->view)
        return;

    //JGG qDebug() << "1";
    if(data->description().contains("vtkDataMesh"))
    {
        if (medMeshAbstractViewInteractor *interactor = dynamic_cast<medMeshAbstractViewInteractor*>(d->view->interactor ("v3dViewMeshInteractor")))
            if (!d->interactors.contains (interactor))
            {
                d->interactors.append (interactor);
            }
            if (medMeshAbstractViewInteractor *interactor = dynamic_cast<medMeshAbstractViewInteractor*>(d->view->interactor ("v3dViewMeshInteractor")))
            {
                d->currentInteractor = d->interactors.indexOf(interactor);

            }
        this->constructMeshLayer(data, d->view->meshLayerCount());
        d->view->setMeshLayerCount(d->view->meshLayerCount()+1);
    }
    else
    {
        this->constructImageLayer(data, layer);
    }

    //decide whether to show the 2 layers slider:
    raiseSlider(d->view->layerCount() == 2);
}

void
    medViewerToolBoxViewProperties::clear(void)
{
    d->currentLayer = 0;
    d->propertiesTree->clear();
    raiseSlider(false);
    d->view = 0;
}
void medViewerToolBoxViewProperties::onColorChanged(int selection)
{
    QColor color;

    switch(selection)
    {
    case 1: color = QColor("#000000");break;
    case 2: color = QColor("#FFFFFF");break;
    case 3: color = QColor("#808080");break;
    case 4: color = QColor("#800000");break;
    case 5: color = QColor("#804040");break;
    case 6: color = QColor("#FF8080");break;
    case 7: color = QColor("#FF0000");break;
    case 8: color = QColor("#FFFF80");break;
    case 9: color = QColor("#FFFF00");break;
    case 10: color = QColor("#FF8040");break;
    case 11: color = QColor("#FF8000");break;
    case 12: color = QColor("#80FF80");break;
    case 13: color = QColor("#80FF00");break;
    case 14: color = QColor("#00FF00");break;
    case 15: color = QColor("#80FFFF");break;
    case 16: color = QColor("#00FFFF");break;
    case 17: color = QColor("#004080");break;
    case 18: color = QColor("#0000FF");break;
    case 19: color = QColor("#0080FF");break;
    case 20: color = QColor("#0080C0");break;
    }
    d->interactors[d->currentInteractor]->setLayer(d->view->currentMeshLayer());
    d->interactors[d->currentInteractor]->onColorPropertySet(color);
}
void
    medViewerToolBoxViewProperties::onVisibilitySet(int state)
{

    if (!d->view)
        return;
    if(!d->isMesh)
    {
        if (state == Qt::Checked)
            d->view->setVisibility(1, d->currentLayer);
        else
            d->view->setVisibility(0, d->currentLayer);
    }
    else
    {
        d->interactors[d->currentInteractor]->setLayer(d->view->currentMeshLayer());
        if (state == Qt::Checked)
            d->interactors[d->currentInteractor]->setProperty("Visibility","true");
        else
            d->interactors[d->currentInteractor]->setProperty("Visibility","false");
    }
    d->view->update();
}

void medViewerToolBoxViewProperties::onColorSelected(const QColor& color)
{
    if (!d->view)
        return;
    if(!d->isMesh)
        return;
    d->interactors[d->currentInteractor]->setLayer(d->view->currentMeshLayer());

}

void
    medViewerToolBoxViewProperties::onEdgeVisibilitySet(int state)
{
    if (!d->view)
        return;

    d->interactors[d->currentInteractor]->setLayer(d->view->currentMeshLayer());
    if (state == Qt::Checked)
        d->interactors[d->currentInteractor]->setProperty("ShowEdges","true");
    else
        d->interactors[d->currentInteractor]->setProperty("ShowEdges","false");
    d->view->update();
}

void
    medViewerToolBoxViewProperties::onOpacitySliderSet(int opacity)
{
    if (!d->view)
        return;

    double d_opacity = static_cast<double> (opacity) / 100.0;
    if(!d->isMesh)
        d->view->setOpacity(d_opacity, d->currentLayer);
    else
    {
        d->interactors[d->currentInteractor]->setLayer( d->view->currentMeshLayer());
        d->interactors[d->currentInteractor]->setOpacity(d_opacity);
    }

    d->view->update();
}


void
    medViewerToolBoxViewProperties::on2LayersOpacitySliderSet(int opacity)
{
    if (!d->view)
        return;

    double d_opacity = static_cast<double> (opacity) / 100.0;
    d->view->setOpacity(d_opacity, 1);
    d->view->setOpacity(1.0 - d_opacity, 0);
    d->view->update();
}
void medViewerToolBoxViewProperties::onAttrBoxChanged(int index)
{

    d->interactors[d->currentInteractor]->setAttribute(d->attributeList[index], d->view->currentMeshLayer());
    if (!d->view)
        return;
    if(!d->isMesh)
        return;

    QComboBox * lutBox = d->meshLutBoxList[d->view->currentMeshLayer()];
    if(index!=0 && lutBox->count()==0)
    {
    lutBox->addItem("Default");
    lutBox->addItem("Black & White");
    lutBox->addItem("Black & White Inversed");
    lutBox->addItem("Spectrum");
    lutBox->addItem("Hot Metal");
    lutBox->addItem("Hot Green");
    lutBox->addItem("Hot Iron");
    lutBox->addItem("GE");
    lutBox->addItem("Flow");
    lutBox->addItem("Loni");
    lutBox->addItem("Loni 2");
    lutBox->addItem("Asymmetry");
    lutBox->addItem("P-Value");
    lutBox->addItem("Red Black Alpha");
    lutBox->addItem("Green Black Alpha");
    lutBox->addItem("Blue Black Alpha");
    lutBox->addItem("Muscles & Bones");
    lutBox->addItem("Bones");
    lutBox->addItem("Red Vessels");
    lutBox->addItem("Cardiac");
    lutBox->addItem("Gray Rainbow");
    lutBox->addItem("Stern");
    lutBox->addItem("Black Body");
    }
    if(index != 0)
    {
        lutBox->setEnabled(true);
        d->interactors[d->currentInteractor]->setScalarVisibility(true);
        lutBox->setCurrentIndex(lutBox->findText(*(d->interactors[d->currentInteractor]->lut(d->view->currentMeshLayer()))));
    }
    else
    {
        lutBox->setEnabled(false);
        d->interactors[d->currentInteractor]->setLayer( d->view->currentMeshLayer());
        d->interactors[d->currentInteractor]->setScalarVisibility(false);
    }
}

void
    medViewerToolBoxViewProperties::onLUTChanged(int index)
{

    if (!d->view)
        return;
    if(!d->isMesh)
    {
        d->view->setCurrentLayer(d->currentLayer);
        d->view->setProperty("LookupTable", d->lutList.at(index));
    }
    else
    {
        d->interactors[d->currentInteractor]->setLayer( d->view->currentMeshLayer());
        d->interactors[d->currentInteractor]->setProperty("LUTMode", d->lutList.at(index));
    }
    d->view->update();
}

void
    medViewerToolBoxViewProperties::onPresetChanged(int index)
{
    if (!d->view)
        return;
    if(d->isMesh)
        return;

    d->view->setCurrentLayer(d->currentLayer);
    d->view->setProperty("Preset", d->presetList.at(index));
    d->view->update();
}

void
    medViewerToolBoxViewProperties::onRenderingChanged(int index)
{
    if (!d->view)
        return;
    d->interactors[d->currentInteractor]->setLayer( d->view->currentMeshLayer());
    d->interactors[d->currentInteractor]->setProperty("RenderingMode", d->renderingList.at(index));
    d->view->update();
}

void
    medViewerToolBoxViewProperties::onItemClicked(QTreeWidgetItem * item, int column)
{

    d->propertiesTree->clearSelection();
    if (item->type() == QTreeWidgetItem::UserType + 1)
    {
        if (item->isExpanded())
            d->propertiesTree->collapseItem(item);
        else
        {
            d->propertiesTree->collapseAll();
            d->propertiesTree->expandItem(item);

            d->currentLayer = item->text(0).toInt();

            if(item->text(0).contains("Mesh"))
            {
                QString s = item->text(0).remove(0,5);
                d->view->setCurrentMeshLayer(s.toInt());
                d->isMesh = true;
            }
            else
            {
                d->isMesh = false;
            }
        }
    }
}

void medViewerToolBoxViewProperties::onContextTreeMenu( const QPoint point )
{

    QTreeWidgetItem * item = 0;
    item = d->propertiesTree->itemAt(point);

    if (!item)
        return;

    if (item->type() != QTreeWidgetItem::UserType + 1)
        return;

    if (item->text(0).toInt() == 0)
        return;

    d->currentLayer = item->text(0).toInt();

    item->setSelected(true);

    QMenu * menu = new QMenu(d->propertiesTree);
    menu->setFocusPolicy(Qt::NoFocus);
    QAction * deleteLayer = new QAction(this);
    deleteLayer->setIcon(QIcon(":icons/cross.png"));
    deleteLayer->setIconVisibleInMenu(true);
    deleteLayer->setText(tr("Delete"));
    QObject::connect(deleteLayer, SIGNAL(triggered()), this, SLOT(onDeleteLayer()));
    menu->addAction(deleteLayer);

    menu->exec(d->propertiesTree->mapToGlobal(point));
    delete menu;
}

void medViewerToolBoxViewProperties::onDeleteLayer()
{
    d->view->removeOverlay(d->currentLayer);
    d->view->update();

    if (d->currentLayer != 0)
        d->propertiesTree->invisibleRootItem()->removeChild(d->propertiesTree->selectedItems()[0]);

    d->propertiesTree->clearSelection();
    raiseSlider(d->view->layerCount() == 2);
}


void medViewerToolBoxViewProperties::raiseSlider(bool isVisible,double opacity)
{
    if (isVisible)
    {
        if (opacity < 0 || opacity > 1)
        {
            opacity = d->view->opacity(1);
        }

        //set opacity to layer 1's
        d->slider->setValue( opacity * 100);
        d->twoLayersWidget->show();
    }
    else
    {
        d->twoLayersWidget->hide();
    }
}

void medViewerToolBoxViewProperties::onSwitchLayersButtonClicked()
{
    if (d->switchLayersButton->text() == d->textLayer0)
    {
        d->switchLayersButton->setText(d->textLayer1);
        raiseSlider(true,0.0);
    }
    else
    {
        d->switchLayersButton->setText(d->textLayer0);
        raiseSlider(true,1.0);
    }
}

QIcon medViewerToolBoxViewProperties::createIcon(QString colorName)
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(QColor(colorName));
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}

void medViewerToolBoxViewProperties::onWindowingChanged(bool checked)
{
    if (d->view) {
        d->view->setProperty("MouseInteraction", "Windowing");
    }
}

void medViewerToolBoxViewProperties::onZoomingChanged(bool checked)
{
    if (d->view) {
        d->view->setProperty("MouseInteraction", "Zooming");
    }
}

void medViewerToolBoxViewProperties::onSlicingChanged(bool checked)
{
    if (d->view) {
        d->view->setProperty("MouseInteraction", "Slicing");
    }
}

void medViewerToolBoxViewProperties::onMeasuringChanged(bool checked)
{
    if (d->view) {
        d->view->setProperty("MouseInteraction", "Measuring");
    }
}


void medViewerToolBoxViewProperties::onAxisVisibilityChanged(bool visible)
{
    if (d->view) {
        d->view->setProperty("ShowAxis", (visible ? "true" : "false"));
        d->view->update();
    }
}

void medViewerToolBoxViewProperties::onScalarBarVisibilityChanged(bool visible)
{
    if (d->view) {
        d->view->setProperty("ShowScalarBar", (visible ? "true" : "false"));
        d->view->update();
    }
}

void medViewerToolBoxViewProperties::onRulerVisibilityChanged(bool visible)
{
    if (d->view) {
        d->view->setProperty("ShowRuler", (visible ? "true" : "false"));
        d->view->update();
    }
}

void medViewerToolBoxViewProperties::onAnnotationsVisibilityChanged(bool visible)
{
    if (d->view) {
        d->view->setProperty("ShowAnnotations", (visible ? "true" : "false"));
        d->view->update();
    }
}






void medViewerToolBoxViewProperties::on2DTriggered(dtkAbstractView* view)
{
    if(view->property("Orientation")=="Axial" ||
        view->property("Orientation")=="Sagittal" ||
        view->property("Orientation")=="Coronal")
    {
        d->propertiesView->show();
        //d->propView->show();
        d->view3dToolBoxWidget->hide();
    }
}
void medViewerToolBoxViewProperties::on3DTriggered(dtkAbstractView* view)
{
    if(view->property("Orientation")=="3D")
    {
        d->propertiesView->hide();
        //d->propView->hide();
        d->view3dToolBoxWidget->show();
    }
}

void medViewerToolBoxViewProperties::onModeChanged(QString mode)
{
    if (d->view) {
        d->view->blockSignals (true);
        d->view->setProperty("3DMode", mode);
        d->view->blockSignals (false);
        d->view->update();
    }
}

void medViewerToolBoxViewProperties::onVRModeChanged(QString mode)
{
    if (d->view) {
        d->view->blockSignals (true);
        d->view->setProperty("Renderer", mode);
        d->view->blockSignals (false);
        d->view->update();
    }
}
void medViewerToolBoxViewProperties::onLodChanged(int value)
{
    if (d->view) {
        d->view->blockSignals (true);
        d->view->setMetaData("LOD", QString::number(value));
        d->view->blockSignals (false);
        d->view->update();
    }
}
void medViewerToolBoxViewProperties::onCroppingChanged(bool checked)
{
    if (d->view) {
        d->view->blockSignals (true);
        d->view->setProperty("Cropping", (checked ? "true" : "false"));
        d->view->blockSignals (false);
        d->view->update();
    }
}
