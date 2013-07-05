/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewPropertiesToolBox.h>

#include <dtkCore/dtkAbstractData.h>

#include <medStorage.h>
#include <medSettingsManager.h>
#include <medMetaDataKeys.h>
#include <medAbstractView.h>
#include <medToolBoxTab.h>
#include <medMeshAbstractViewInteractor.h>

#define LAYER_NUMBER_ROLE (Qt::UserRole+1)
#define LAYER_IS_MESH_ROLE (Qt::UserRole+2)

//Just a normal combobox, but with a link to its associated LutBox.
//The selected item in the attribute box determines which Lut is selected
//or if a lut can even be selected.
class medAttributeBox: public QComboBox
{
public:
    medAttributeBox(QWidget* parent,QComboBox* lutBox);
    medAttributeBox():QComboBox(){}
    void setLutBox(QComboBox* lutBox);
    QComboBox* getLutBox()const;
protected:
    QWeakPointer<QComboBox> lutBox;
};

medAttributeBox::medAttributeBox(QWidget *parent, QComboBox *lutBox):
    QComboBox(parent)
{
    this->lutBox = lutBox;
}

void medAttributeBox::setLutBox(QComboBox *lutBox)
{
    this->lutBox = lutBox;
}

QComboBox* medAttributeBox::getLutBox()const
{
    return lutBox.data();
}

class medViewPropertiesToolBoxPrivate
{
public:

    QTreeWidget * propertiesTree;
    QWidget * propertiesView;
    QWidget * propView;
    //attribtueList can be filled depending on the data, not static.
    QList<QStringList> attributeList;
    static QStringList lutList;
    static QStringList presetList;
    static QMap<QString, QString> presetToLut;
    static QStringList renderingList;
    int currentLayer;
    QWidget * twoLayersWidget;
    QSlider * slider;
    QString thumbnailLocation;
    QString textLayer0;
    QString textLayer1;
    QPushButton * switchLayersButton;
    medMeshAbstractViewInteractor * meshInteractor;
    medAbstractView *view;

    QButtonGroup *mouseGroup;
    QPushButton *windowingPushButton;
    QPushButton *zoomingPushButton;
    QPushButton *slicingPushButton;
    QPushButton *measuringPushButton;

    QPushButton *axialButton;
    QPushButton *coronalButton;
    QPushButton *sagittalButton;
    QPushButton *view3DButton;

    QCheckBox *scalarBarVisibilityCheckBox;
    QCheckBox *axisVisibilityCheckBox;
    QCheckBox *rulerVisibilityCheckBox;
    QCheckBox *annotationsVisibilityCheckBox;

    QComboBox * view3dModeComboBox;
    QComboBox * view3dVRModeComboBox;
    QSlider * view3dLODSlider;
    QPushButton * croppingPushButton;
    QCheckBox * view3dDepthPeelingCheckBox;
    QWidget * view3dToolBoxWidget;

    QString thumbLocation;

    QTreeWidgetItem * layerItem;
    bool isMesh;
};

QStringList medViewPropertiesToolBoxPrivate::lutList;
QStringList medViewPropertiesToolBoxPrivate::renderingList;
QStringList medViewPropertiesToolBoxPrivate::presetList;
QMap<QString, QString> medViewPropertiesToolBoxPrivate::presetToLut;

medViewPropertiesToolBox::medViewPropertiesToolBox(QWidget *parent) :
medToolBox(parent), d(new medViewPropertiesToolBoxPrivate)
{

    d->view = 0;
    d->meshInteractor = 0;
    d->currentLayer = 0;
    d->textLayer0 = tr("Switch to layer 0 only");
    d->textLayer1 = tr("Switch to layer 1 only");
    d->isMesh = false;
    if (d->lutList.isEmpty())
    {
        d->lutList << "Default" << "Black & White" << "Black & White Inversed"
                   << "Spectrum" << "Hot Metal" << "Hot Green"
                   << "Hot Iron" << "GE" << "Flow" << "Loni" << "Loni 2"
                   << "Asymmetry" << "P-Value" << "Red Black Alpha"
                   << "Green Black Alpha" << "Blue Black Alpha"
                   << "Muscles & Bones" << "Bones" << "Red Vessels"
                   << "Cardiac" << "Gray Rainbow" << "Stern" << "Black Body";
    }
    if (d->presetList.isEmpty())
    {
        d->presetList << "None" << "VR Muscles&Bones" << "Vascular I"
                      << "Vascular II" << "Vascular III" << "Vascular IV"
                      << "Standard" << "Soft" << "Soft on White"
                      << "Soft on Blue" << "Red on White" << "Glossy" ;
    }

    // a preset is a predefined choice of three things: LUT, window width and window level
    // hence when the user selects a preset we also change the LUT
    if(d->presetToLut.isEmpty())
    {
        d->presetToLut.insert("None", "Black & White");
        d->presetToLut.insert("VR Muscles&Bones", "Muscles & Bones");
        d->presetToLut.insert("Vascular I", "Stern");
        d->presetToLut.insert("Vascular II", "Red Vessels");
        d->presetToLut.insert("Vascular III", "Red Vessels");
        d->presetToLut.insert("Vascular IV", "Red Vessels");
        d->presetToLut.insert("Standard", "Muscles & Bones");
        d->presetToLut.insert("Soft", "Bones");
        d->presetToLut.insert("Soft on White", "Muscles & Bones");
        d->presetToLut.insert("Soft on Blue", "Muscles & Bones");
        d->presetToLut.insert("Red on White", "Red Vessels");
        d->presetToLut.insert("Glossy", "Bones");
    }
    if (d->renderingList.isEmpty())
    {
        d->renderingList << "wireframe"<<"surface"<<"points";
    }

    d->propertiesTree = new QTreeWidget(this);
    d->propertiesTree->setFocusPolicy(Qt::NoFocus);

    d->propertiesTree->setColumnCount(3);
    d->propertiesTree->setColumnWidth(0,85);
    d->propertiesTree->setColumnWidth(1,65);
    d->propertiesTree->setSelectionMode(QAbstractItemView::NoSelection);
    QStringList headers;
    headers << tr("Object") << tr("Name") << tr("Value");
    d->propertiesTree->setHeaderLabels(headers);
    d->propertiesTree->setAnimated(true);
    d->propertiesTree->setAlternatingRowColors(true);
    d->propertiesTree->setRootIsDecorated(true);
    d->propertiesTree->setContextMenuPolicy(Qt::CustomContextMenu);

    this->setTitle(tr("View Properties"));

    QObject::connect(d->propertiesTree, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
                     this, SLOT(onItemClicked(QTreeWidgetItem *)));
    QObject::connect(d->propertiesTree, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(onContextTreeMenu(QPoint)));
    QObject::connect(d->propertiesTree,SIGNAL(itemExpanded(QTreeWidgetItem*)),
                     this, SLOT(onItemClicked(QTreeWidgetItem *)));

    d->propertiesView = new QWidget(this);

    d->windowingPushButton = new QPushButton("", this);
    d->windowingPushButton->setFocusPolicy(Qt::NoFocus);
    d->windowingPushButton->setIcon (QIcon (":/icons/wlww.png"));
    d->windowingPushButton->setCheckable (true);
    d->windowingPushButton->setMinimumWidth ( 20 );
    d->windowingPushButton->setToolTip (tr("Windowing"));
    d->zoomingPushButton   = new QPushButton("", this);
    d->zoomingPushButton->setFocusPolicy(Qt::NoFocus);
    d->zoomingPushButton->setIcon (QIcon (":/icons/magnify.png"));
    d->zoomingPushButton->setCheckable (true);
    d->zoomingPushButton->setToolTip (tr("Zooming"));
    d->slicingPushButton   = new QPushButton("", this);
    d->slicingPushButton->setFocusPolicy(Qt::NoFocus);
    d->slicingPushButton->setIcon (QIcon (":/icons/stack.png"));
    d->slicingPushButton->setCheckable (true);
    d->slicingPushButton->setToolTip (tr("Slicing"));
    d->measuringPushButton = new QPushButton("", this);
    d->measuringPushButton->setFocusPolicy(Qt::NoFocus);
    d->measuringPushButton->setIcon (QIcon (":/icons/length.png"));
    d->measuringPushButton->setCheckable (true);
    d->measuringPushButton->setToolTip (tr("Measuring"));

    d->mouseGroup = new QButtonGroup (this);
    d->mouseGroup->addButton ( d->windowingPushButton );
    d->mouseGroup->addButton ( d->zoomingPushButton );
    d->mouseGroup->addButton ( d->slicingPushButton );
    d->mouseGroup->addButton ( d->measuringPushButton );
    d->mouseGroup->setExclusive (true);

#if defined(Q_WS_MAC)
#  define CTRL_KEY "Cmd"
#else
#   define CTRL_KEY "Ctrl"
#endif

    // Orientation buttons //
    d->axialButton = new QPushButton(this);
    d->axialButton->setIcon(QIcon(":/icons/AxialIcon.png"));
    d->axialButton->setToolTip(tr("Axial view (" CTRL_KEY " + 1)"));
    d->axialButton->setCheckable(true);
    d->axialButton->setMinimumHeight(45);
    d->axialButton->setIconSize(QSize(40,40));
    d->axialButton->setShortcut(Qt::CTRL + Qt::Key_1);
    d->coronalButton = new QPushButton(this);
    d->coronalButton->setIcon(QIcon(":/icons/CoronalIcon.png"));
    d->coronalButton->setToolTip(tr("Coronal view (" CTRL_KEY " + 2)"));
    d->coronalButton->setCheckable(true);
    d->coronalButton->setMinimumHeight(45);
    d->coronalButton->setIconSize(QSize(40,40));
    d->coronalButton->setShortcut(Qt::CTRL + Qt::Key_2);
    d->sagittalButton = new QPushButton(this);
    d->sagittalButton->setIcon(QIcon(":/icons/SagittalIcon.png"));
    d->sagittalButton->setToolTip(tr("Sagittal view (" CTRL_KEY " + 3)"));
    d->sagittalButton->setCheckable(true);
    d->sagittalButton->setMinimumHeight(45);
    d->sagittalButton->setIconSize(QSize(45,45));
    d->sagittalButton->setShortcut(Qt::CTRL + Qt::Key_3);
    d->view3DButton = new QPushButton(this);
    d->view3DButton->setIcon(QIcon(":/icons/3DIcon.png"));
    d->view3DButton->setToolTip(tr("3D view (" CTRL_KEY " + 4)"));
    d->view3DButton->setCheckable(true);
    d->view3DButton->setMinimumHeight(45);
    d->view3DButton->setIconSize(QSize(40,40));
    d->view3DButton->setShortcut(Qt::CTRL + Qt::Key_4);

    QButtonGroup *orientationButtonGroup = new QButtonGroup(this);
    orientationButtonGroup->addButton(d->axialButton);
    orientationButtonGroup->addButton(d->coronalButton);
    orientationButtonGroup->addButton(d->sagittalButton);
    orientationButtonGroup->addButton(d->view3DButton);
    orientationButtonGroup->setExclusive(true);

    setCurrentInteractionFromSettings();

    d->propView = new QWidget(this);
    QHBoxLayout * propLayout = new QHBoxLayout;

    d->scalarBarVisibilityCheckBox = new QCheckBox(this);
    d->scalarBarVisibilityCheckBox->setFocusPolicy(Qt::NoFocus);
    d->scalarBarVisibilityCheckBox->setToolTip(tr("Show scalar bar"));
    d->axisVisibilityCheckBox = new QCheckBox(this);
    d->axisVisibilityCheckBox->setFocusPolicy(Qt::NoFocus);
    d->axisVisibilityCheckBox->setToolTip(tr("Show axes"));
    d->rulerVisibilityCheckBox = new QCheckBox(this);
    d->rulerVisibilityCheckBox->setFocusPolicy(Qt::NoFocus);
    d->annotationsVisibilityCheckBox = new QCheckBox(this);
    d->annotationsVisibilityCheckBox->setFocusPolicy(Qt::NoFocus);

    propLayout->addWidget(d->scalarBarVisibilityCheckBox);
    propLayout->addWidget(d->annotationsVisibilityCheckBox);
    d->propView->setLayout(propLayout);
    propLayout->setAlignment(Qt::AlignJustify);
    d->scalarBarVisibilityCheckBox->setText(tr("Scalar Bar"));
    d->axisVisibilityCheckBox->setText(tr("Axes"));
    d->rulerVisibilityCheckBox->setText(tr("Ruler"));
    d->rulerVisibilityCheckBox->setToolTip(tr("Show ruler"));
    d->annotationsVisibilityCheckBox->setText("Annotations");
    d->annotationsVisibilityCheckBox->setToolTip(tr("Show annotations"));
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

    connect(d->windowingPushButton, SIGNAL(toggled(bool)),
            this, SLOT(onWindowingChanged(bool)));
    connect(d->zoomingPushButton, SIGNAL(toggled(bool)),
            this, SLOT(onZoomingChanged(bool)));
    connect(d->slicingPushButton, SIGNAL(toggled(bool)),
            this, SLOT(onSlicingChanged(bool)));
    connect(d->measuringPushButton, SIGNAL(toggled(bool)),
            this, SLOT(onMeasuringChanged(bool)));

    connect(d->axialButton,SIGNAL(toggled(bool)),this,SLOT(onAxialChanged(bool)));
    connect(d->coronalButton,SIGNAL(toggled(bool)),this,SLOT(onCoronalChanged(bool)));
    connect(d->sagittalButton,SIGNAL(toggled(bool)),this,SLOT(onSagittalChanged(bool)));
    connect(d->view3DButton,SIGNAL(toggled(bool)),this,SLOT(onView3DChanged(bool)));

    QWidget *orientationButtons = new QWidget(this);
    QHBoxLayout *orientationButtonLayout = new QHBoxLayout;
    orientationButtonLayout->addWidget(d->axialButton);
    orientationButtonLayout->addWidget(d->coronalButton);
    orientationButtonLayout->addWidget(d->sagittalButton);
    orientationButtonLayout->addWidget(d->view3DButton);
    orientationButtons->setLayout(orientationButtonLayout);

    d->view3dModeComboBox = new QComboBox(this);
    d->view3dModeComboBox->setFocusPolicy(Qt::NoFocus);
    d->view3dModeComboBox->setToolTip(tr("Choose a 3D mode (e.g. Volume Rendering, Maximum Intensity Projection, MultiPlanar Reconstruction)"));
    d->view3dModeComboBox->addItem("VR");
    d->view3dModeComboBox->addItem("MIP - Maximum");
    d->view3dModeComboBox->addItem("MIP - Minimum");
    d->view3dModeComboBox->addItem("MPR");
    d->view3dModeComboBox->addItem("Off");

    d->view3dVRModeComboBox = new QComboBox(this);
    d->view3dVRModeComboBox->setFocusPolicy(Qt::NoFocus);
    d->view3dVRModeComboBox->setToolTip(tr("Choose among rendering techniques (e.g. GPU accelerated rendering, Ray Casting)"));
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
    d->croppingPushButton->setToolTip(tr("Crop volume tool"));

    d->view3dDepthPeelingCheckBox = new QCheckBox ("", this);
    d->view3dDepthPeelingCheckBox->setFocusPolicy(Qt::NoFocus);
    d->view3dDepthPeelingCheckBox->setToolTip(tr("Activate depth-peeling to have a proper opacity rendering"));

    connect(d->view3dModeComboBox,   SIGNAL(currentIndexChanged(QString)), this, SLOT(onModeChanged(QString)));
    connect(d->view3dVRModeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onVRModeChanged(QString)));
    connect(d->view3dLODSlider,      SIGNAL(valueChanged(int)),            this, SLOT(onLodChanged(int)));
    connect(d->croppingPushButton,   SIGNAL(toggled(bool)),                this, SLOT(onCroppingChanged(bool)));
    connect(d->view3dDepthPeelingCheckBox,   SIGNAL(toggled(bool)),        this, SLOT(onDepthPeelingChanged(bool)));

    d->view3dToolBoxWidget = new QWidget(this);
    QFormLayout *view3dToolBoxWidgetLayout = new QFormLayout(d->view3dToolBoxWidget);
    view3dToolBoxWidgetLayout->addRow(tr("3D Mode"), d->view3dModeComboBox);
    view3dToolBoxWidgetLayout->addRow(tr("Renderer:"), d->view3dVRModeComboBox);
    view3dToolBoxWidgetLayout->addRow(tr("LOD:"), d->view3dLODSlider);
    view3dToolBoxWidgetLayout->addRow(tr("Cropping:"), d->croppingPushButton);
    view3dToolBoxWidgetLayout->addRow(tr("Depth Peeling:"), d->view3dDepthPeelingCheckBox);
    view3dToolBoxWidgetLayout->setFormAlignment(Qt::AlignHCenter);

    //add 2 layers opacity slider and switcher.
    d->twoLayersWidget = new QWidget(this);
    QGridLayout * twoLayersLayout = new QGridLayout(d->twoLayersWidget);
    //d->twoLayersWidget->setLayout(twoLayersLayout);

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

    this->addWidget(orientationButtons);
    this->addWidget(d->propertiesView);
    this->addWidget(d->view3dToolBoxWidget);
    d->view3dToolBoxWidget->hide();
    this->addWidget(d->propertiesTree);
    this->addWidget(d->twoLayersWidget);
    d->twoLayersWidget->hide();
    this->addWidget(d->propView);
}

medViewPropertiesToolBox::~medViewPropertiesToolBox(void)
{
    delete d;
    d = NULL;
}

void medViewPropertiesToolBox::update(dtkAbstractView *view)
{
    medToolBox::update(view);
    if(!view)
    {
        clear();
        return;
    }

    medAbstractView * medView = dynamic_cast<medAbstractView *> (view);
    if ( !medView )
        return;

    if ((d->view) && (d->view != medView) )
    {
        d->view->disconnect(this,0);
        clear();
    }

    d->view = medView;
    d->propertiesTree->clear();


    double layer1Opacity = 0.5;
    //retrieve layer1 opacity, if possible
    if(d->view->layerCount() == 2)
        layer1Opacity = d->view->opacity(1) / (d->view->opacity(0) + d->view->opacity(1));
    //decide whether to show the 2 layers slider
    raiseSlider(d->view->layerCount() == 2, layer1Opacity);

    if ( ! d->meshInteractor )
        d->meshInteractor = dynamic_cast<medMeshAbstractViewInteractor*>(d->view->interactor ("v3dViewMeshInteractor"));

    //Loop all layers and create a layer icon on the toolbox with respect to the fact that it is a mesh or an image
    for (int i = 0, meshNumber = 0, imageNumber = 0; i < d->view->layerCount() + d->view->meshLayerCount(); i++)
    {
        if(d->view->dataInList(i) && d->view->dataInList(i)->identifier().contains("vtkDataMesh"))
        {
            this->constructMeshLayer(d->view->dataInList(i), meshNumber);
            meshNumber++;
        }
        else //if (d->view->dataInList(i))
        {
            this->constructImageLayer(d->view->dataInList(i), imageNumber);
            imageNumber++;
        }
    }

    // select the current layer
    if (d->propertiesTree->topLevelItem(d->currentLayer))
            d->propertiesTree->topLevelItem(d->currentLayer)->setExpanded(true);


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

    if (d->view->property("Orientation") == "Axial")
        d->axialButton->setChecked(true);
    else if (d->view->property("Orientation") == "Coronal")
        d->coronalButton->setChecked(true);
    else if (d->view->property("Orientation") == "Sagittal")
        d->sagittalButton->setChecked(true);
    else
        d->view3DButton->setChecked(true);
}

void medViewPropertiesToolBox::constructImageLayer(dtkAbstractData* data, int imageLayer)
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

    if (data) ///put thumbnail
    {
        if (medMetaDataKeys::SeriesThumbnail.is_set_in(data))
        {
            d->thumbLocation = medMetaDataKeys::SeriesThumbnail.getFirstValue(data,":icons/layer.png");
        }
    }

    d->layerItem = new QTreeWidgetItem(d->propertiesTree->invisibleRootItem(), QTreeWidgetItem::UserType+1);
    d->layerItem->setText(0, QString::number(imageLayer));
    d->layerItem->setIcon(0,QIcon(d->thumbLocation));
    if (data != NULL && medMetaDataKeys::SeriesDescription.is_set_in(data))
    {
        d->layerItem->setToolTip(0,data->metaDataValues(medMetaDataKeys::PatientName.key())[0]
        + "\n" + data->metaDataValues(medMetaDataKeys::StudyDescription.key())[0]
        + "\n" + data->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0]);
    }
    d->layerItem->setData(0, LAYER_NUMBER_ROLE, imageLayer);
    d->layerItem->setData(0, LAYER_IS_MESH_ROLE, false);

    QTreeWidgetItem * visibleItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    visibleItem->setText(1, tr("Visible"));
    //No need to parent this widget, setItemWidget takes ownership of the widget
    QCheckBox * visibleBox = new QCheckBox();
    visibleBox->setChecked(d->view->visibility(imageLayer));
    d->propertiesTree->setItemWidget(visibleItem, 2, visibleBox);


    QTreeWidgetItem * opacityItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    opacityItem->setText(1, tr("Opacity"));
    //No need to parent this widget, setItemWidget takes ownership of the widget
    QSlider * opacityBox = new QSlider(Qt::Horizontal);
    opacityBox->setRange(0,100);
    opacityBox->setValue(d->view->opacity(imageLayer) * 100);
    d->propertiesTree->setItemWidget(opacityItem, 2, opacityBox);

    if(medMetaDataKeys::SeriesType.getFirstValue(data)!="Mask")
    {
        QTreeWidgetItem * lutItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
        lutItem->setText(1, tr("LUT"));
        //No need to parent this widget, setItemWidget takes ownership of the widget
        QComboBox * lutBox = new QComboBox();
        lutBox->setFocusPolicy(Qt::NoFocus);
        foreach (QString lut,d->lutList)
        {
            lutBox->addItem(lut);
        }

        d->propertiesTree->setItemWidget(lutItem, 2, lutBox);
        lutBox->setCurrentIndex(lutBox->findText(d->view->getLUT(imageLayer)));

        QTreeWidgetItem * presetItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
        presetItem->setText(1, tr("Preset"));
        QComboBox * presetBox = new QComboBox();
        presetBox->setFocusPolicy(Qt::NoFocus);
        foreach(QString preset,d->presetList)
        {
             presetBox->addItem(preset);
        }

        d->propertiesTree->setItemWidget(presetItem, 2, presetBox);
        presetBox->blockSignals(true);
        presetBox->setCurrentIndex(presetBox->findText(d->view->getPreset(imageLayer)));
        presetBox->blockSignals(false);

        QObject::connect(lutBox, SIGNAL(currentIndexChanged(int)),
                         this, SLOT(onLUTChanged(int)));
        QObject::connect(presetBox, SIGNAL(currentIndexChanged(int)),
                         this, SLOT(onPresetChanged(int)));

    }

    QObject::connect(visibleBox, SIGNAL(stateChanged(int)),
                     this, SLOT(onVisibilitySet(int)));
    QObject::connect(opacityBox, SIGNAL(valueChanged(int)),
                     this, SLOT(onOpacitySliderSet(int)));
    //d->propertiesTree->collapseAll();
}

void medViewPropertiesToolBox::constructMeshLayer(dtkAbstractData* data, int meshLayer)
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
    {
        if (medMetaDataKeys::SeriesThumbnail.is_set_in(data))
            d->thumbLocation = medMetaDataKeys::SeriesThumbnail.getFirstValue(data,":icons/layer.png");
    }

    d->layerItem = new QTreeWidgetItem(d->propertiesTree->invisibleRootItem(), QTreeWidgetItem::UserType+1);
    d->layerItem->setText(0, layerItemString);
    d->layerItem->setIcon(0,QIcon(d->thumbLocation));
    if (medMetaDataKeys::SeriesDescription.is_set_in(data)) {
        d->layerItem->setToolTip(0,medMetaDataKeys::PatientName.getFirstValue(data)
        + "\n" + medMetaDataKeys::StudyDescription.getFirstValue(data)
        + "\n" + medMetaDataKeys::SeriesDescription.getFirstValue(data));
    }
    d->layerItem->setData(0, LAYER_NUMBER_ROLE, meshLayer);
    d->layerItem->setData(0, LAYER_IS_MESH_ROLE, true);

    QTreeWidgetItem * meshVisibleItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    meshVisibleItem->setText(1, tr("Visible"));
    QCheckBox * meshVisibleBox = new QCheckBox();
    meshVisibleBox->setChecked(d->meshInteractor->visibility(meshLayer));
    d->propertiesTree->setItemWidget(meshVisibleItem, 2, meshVisibleBox);


    QTreeWidgetItem * opacityItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    opacityItem->setText(1, tr("Opacity"));
    QSlider * opacityBox = new QSlider(Qt::Horizontal);
    opacityBox->setRange(0,100);
    opacityBox->setValue(d->meshInteractor->opacity(meshLayer) * 100);
    d->propertiesTree->setItemWidget(opacityItem, 2, opacityBox);

    //create the lutBox before the attrBox,
    //we want to enable it only if the right attribute is selected
    QComboBox * lutBox = new QComboBox();
    //disabled by default
    lutBox->setEnabled(false);
    lutBox->setFocusPolicy(Qt::NoFocus);
    lutBox->addItems(d->meshInteractor->getAllLUTs());
    lutBox->setCurrentIndex(0);

    QTreeWidgetItem * attrMap = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    attrMap->setText(1, tr("Attributes"));
    medAttributeBox * attrBox = new medAttributeBox(NULL,lutBox);
    attrBox->setFocusPolicy(Qt::NoFocus);
    d->attributeList.append(QStringList());
    d->attributeList[meshLayer] = QStringList("Solid"); // Flo - Ugly, but currently, it's never emptied so we need to reset it
    d->attributeList[meshLayer] << d->meshInteractor->getAllAttributes(meshLayer);
    attrBox->addItems(d->attributeList[meshLayer]);

    d->propertiesTree->setItemWidget(attrMap, 2, attrBox);
    //BEN: Too complex need to rewrite all this!!!!
    attrBox->setCurrentIndex(0);

    QTreeWidgetItem * lutItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    lutItem->setText(1, tr("LUT"));


    d->propertiesTree->setItemWidget(lutItem, 2, lutBox);

    QTreeWidgetItem * edgeVisibleItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    edgeVisibleItem->setText(1, tr("Edge Visible"));
    QCheckBox * edgeVisibleBox = new QCheckBox();
    edgeVisibleBox->setChecked(d->meshInteractor->edgeVisibility(meshLayer));
    d->propertiesTree->setItemWidget(edgeVisibleItem, 2, edgeVisibleBox);


    QTreeWidgetItem * coloringItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    coloringItem->setText(1, tr("Color"));
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

    colorComboBox->setCurrentIndex(colorComboBox->findText(d->meshInteractor->color(meshLayer).name().toUpper()));

    QTreeWidgetItem * renderingItem = new QTreeWidgetItem(d->layerItem, QTreeWidgetItem::UserType+2);
    renderingItem->setText(1, tr("Rendering"));
    QComboBox * renderingBox = new QComboBox();
    renderingBox->setFocusPolicy(Qt::NoFocus);
    //renderingBox->addItem("Default");
    renderingBox->addItem("Wire frame");
    renderingBox->addItem("Surface");
    renderingBox->addItem("Points");

    d->propertiesTree->setItemWidget(renderingItem, 2, renderingBox);
    renderingBox->setCurrentIndex(renderingBox->findText(d->meshInteractor->renderingType(meshLayer), Qt::MatchFixedString));

    QObject::connect(meshVisibleBox, SIGNAL(stateChanged(int)), this, SLOT(onVisibilitySet(int)));
    QObject::connect(opacityBox, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySliderSet(int)));
    QObject::connect(attrBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAttrBoxChanged(int)));
    QObject::connect(lutBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onLUTChanged(int)));
    QObject::connect(edgeVisibleBox, SIGNAL(stateChanged(int)), this, SLOT(onEdgeVisibilitySet(int)));
    QObject::connect(colorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(onColorChanged(int)));
    QObject::connect(renderingBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRenderingChanged(int)));
    //d->propertiesTree->collapseAll();
}
void medViewPropertiesToolBox::onDataAdded(dtkAbstractData* data)
{

}
void medViewPropertiesToolBox::onDataAdded( int layer)
{

}
void medViewPropertiesToolBox::onDataAdded(dtkAbstractData* data,
                                                 int layer)
{

    if(!data)
        return;
    if (!d->view)
        return;

    //JGG qDebug() << "1";
    if(data->identifier().contains("vtkDataMesh"))
    {
        if ( ! d->meshInteractor )
            d->meshInteractor = dynamic_cast<medMeshAbstractViewInteractor*>(d->view->interactor ("v3dViewMeshInteractor"));

        //this solves the layering updating issue and crashes, it is not counting
        //correctly the meshes coz onDataAdded is not connected when the first mesh is
        //loaded, it does not happen when an image is loaded first coz onDataAdded
        //is connected before the first mesh is loaded. Though not best solution in my opinion
        //it works pretty fine and it will do the MR with other toolbox modifications simpler.
        int realMeshCount=0;
        if (d->view->hasImage())
            realMeshCount=d->view->meshLayerCount();
        else
            realMeshCount=d->view->meshLayerCount()+1;
        this->constructMeshLayer(data, realMeshCount);
        d->view->setMeshLayerCount(d->view->meshLayerCount()+1);
    }
    else
    {
        this->constructImageLayer(data, d->view->layerCount()-1);
    }

    //expand the item that has been added.
//    qDebug()<<"let's select item:"<<d->propertiesTree->topLevelItemCount()-1;
    QTreeWidgetItem * newlySelectedItem = d->propertiesTree->topLevelItem(
                d->propertiesTree->topLevelItemCount()-1);
    newlySelectedItem->setExpanded(true);

    //decide whether to show the 2 layers slider:
    raiseSlider(d->view->layerCount() == 2);
}

void medViewPropertiesToolBox::clear(void)
{
    d->currentLayer = 0;
    d->meshInteractor = 0;
    d->propertiesTree->clear();
    raiseSlider(false);
    d->view = 0;
}

void medViewPropertiesToolBox::onColorChanged(int selection)
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
    d->meshInteractor->setColor(d->currentLayer, color);
}

void medViewPropertiesToolBox::onVisibilitySet(int state)
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
        if (state == Qt::Checked)
            d->meshInteractor->setVisibility(d->currentLayer, true);
        else
            d->meshInteractor->setVisibility(d->currentLayer, false);
    }
    d->view->update();
}

void medViewPropertiesToolBox::onColorSelected(const QColor& color)
{
    if (!d->view)
        return;
    if(!d->isMesh)
        return;
}

void medViewPropertiesToolBox::onEdgeVisibilitySet(int state)
{
    if (!d->view)
        return;

    if (state == Qt::Checked)
        d->meshInteractor->setEdgeVisibility(d->currentLayer, true);
    else
        d->meshInteractor->setEdgeVisibility(d->currentLayer, false);
    d->view->update();
}

void medViewPropertiesToolBox::onOpacitySliderSet(int opacity)
{
    if (!d->view)
        return;

    double d_opacity = static_cast<double> (opacity) / 100.0;
    if(!d->isMesh)
        d->view->setOpacity(d_opacity, d->currentLayer);
    else
        d->meshInteractor->setOpacity(d->currentLayer, d_opacity);

    d->view->update();
}


void medViewPropertiesToolBox::on2LayersOpacitySliderSet(int opacity)
{
    if (!d->view)
        return;

    double d_opacity = static_cast<double> (opacity) / 100.0;

    // layer1_op  = 1 if slider > 0.5
    //              2xslider if slider <= 0.5
    double d_opacity1 = std::min(0.5, d_opacity);
    d->view->setOpacity(d_opacity1*2, 1);

    // layer0_op  = 1 if slider < 0.5
    //              1-2x(slider-0.5) if slider >= 0.5
    double d_opacity0 = std::max(0.5, d_opacity);
    d->view->setOpacity(1-(d_opacity0-0.5)*2, 0);
    d->view->update();
}

void medViewPropertiesToolBox::onAttrBoxChanged(int index)
{
    if ( ! d->view || ! d->isMesh)
        return;

    int meshLayer = d->view->currentMeshLayer();
    QString attribute = d->attributeList[meshLayer][index];
    d->meshInteractor->setAttribute(d->currentLayer, attribute);

    medAttributeBox * attrBox = dynamic_cast<medAttributeBox*>(sender());
    attrBox->getLutBox()->setEnabled(index != 0);

    d->view->update();
}
void medViewPropertiesToolBox::disableInteraction(void)
{
    d->view->setProperty("MouseInteraction", "None");
}

void medViewPropertiesToolBox::onLUTChanged(int index)
{
    if (!d->view)
        return;

    if(!d->isMesh)
    {
        d->view->setCurrentLayer(d->currentLayer);
        d->view->setProperty("LookupTable", d->lutList.at(index));


        // as the LUT changed we need to change the preset to None (just the preset combobox)
        // also we won't change the preset back to None if the ww/wl is changed (by design)
        //Only doable on images, Meshes should be left alone.
        QString currentLayerItemString = QString::number(d->currentLayer);
        // we take the first one as there's only one
        QTreeWidgetItem* layerItem = d->propertiesTree->findItems(
                    currentLayerItemString,
                    Qt::MatchExactly | Qt::MatchWrap, 0)[0];
        // the preset item is the fourth one
        QTreeWidgetItem* presetItem = layerItem->child(3);
        QComboBox* presetComboBox = dynamic_cast<QComboBox*>(
                    d->propertiesTree->itemWidget(presetItem, 2));

        // 0 == None
        if (presetComboBox->currentIndex() != 0)
        {
            presetComboBox->blockSignals(true);
            presetComboBox->setCurrentIndex(0);
            presetComboBox->blockSignals(false);
        }
    }
    else
    {
        d->meshInteractor->setLut(d->currentLayer, d->lutList.at(index));
    }

    d->view->update();
}

void medViewPropertiesToolBox::onPresetChanged(int index)
{
    //a "Preset" is a prepared selection of 3 values: LUT, window width and window level

    if (!d->view)
        return;

    // anyone knows why this 'if clause' is here? I mean... a preset sets 3 values: LUT + ww/wl
    // and there IS code for meshes in onLUTchanged (apparently the interactors need to be notified)
    // but if the LUT is set through this function, they are not
//    if(d->isMesh)
//        return;

    // find the LUT combo box to manually change the new value
    QString currentLayerItemString = QString::number(d->currentLayer);
    QTreeWidgetItem* layerItem = d->propertiesTree->findItems(currentLayerItemString, Qt::MatchExactly | Qt::MatchWrap, 0)[0]; // we take the first one as there's only one
    QTreeWidgetItem* lutItem = layerItem->child(2); // the LUT item is the third one
    QComboBox* lutComboBox = dynamic_cast<QComboBox*>(d->propertiesTree->itemWidget(lutItem, 2));
    QString lutStringToSet = d->presetToLut[d->presetList.at(index)];
    int lutValueToSet = d->lutList.indexOf(lutStringToSet, 0);

    lutComboBox->blockSignals(true);
    lutComboBox->setCurrentIndex(lutValueToSet);
    lutComboBox->blockSignals(false);

    if(!d->isMesh)
    {
        d->view->setCurrentLayer(d->currentLayer);
        // the setProperty/LUT is called in the v3dView within setProperty/Preset
        d->view->setProperty("Preset", d->presetList.at(index));
    }
    else
    {
        d->meshInteractor->setLut(d->currentLayer, lutStringToSet);
    }

    d->view->update();
}

void medViewPropertiesToolBox::onRenderingChanged(int index)
{
    if (!d->view)
        return;
    d->meshInteractor->setRenderingType(d->currentLayer, d->renderingList.at(index));
    d->view->update();
}

void medViewPropertiesToolBox::onItemClicked(QTreeWidgetItem * item)
{
    if (d->view && d->view->dataInList(item->text(0).toInt()))
    {
        if(medMetaDataKeys::SeriesType.getFirstValue(d->view->dataInList(item->text(0).toInt()))=="Mask")
        {
            if (d->windowingPushButton->isChecked())
                this->disableInteraction();
        }
        else
        {
            if (d->windowingPushButton->isChecked())
                this->onWindowingChanged(true);
        }
    }

    d->propertiesTree->clearSelection();
    if (item->type() == QTreeWidgetItem::UserType + 1)
    {
    //BEN: we don't try to collapse items, there is always a current layer.
//        if (item->isExpanded())
//        {
//            d->propertiesTree->collapseItem(item);
//        }
//        else
        {
            d->propertiesTree->collapseAll();
            //blocking signak
            d->propertiesTree->blockSignals(true);
            d->propertiesTree->expandItem(item);
            d->propertiesTree->blockSignals(false);

            d->currentLayer = item->data(0, LAYER_NUMBER_ROLE).toInt();
            bool isMesh = item->data(0, LAYER_IS_MESH_ROLE).toBool();
            d->view->setCurrentLayer(d->currentLayer);
            d->view->update();
            if(isMesh)
            {
                d->view->setCurrentMeshLayer(d->currentLayer); //meshlayer is set here
                d->isMesh = true;
            }
            else
            {
                d->isMesh = false;
            }
        }
    }
}

void medViewPropertiesToolBox::onContextTreeMenu( const QPoint point )
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
    deleteLayer->setIcon(QIcon(":icons/cross.svg"));
    deleteLayer->setIconVisibleInMenu(true);
    deleteLayer->setText(tr("Delete"));
    QObject::connect(deleteLayer, SIGNAL(triggered()), this, SLOT(onDeleteLayer()));
    menu->addAction(deleteLayer);

    menu->exec(d->propertiesTree->mapToGlobal(point));
    delete menu;
}

void medViewPropertiesToolBox::onDeleteLayer()
{

    //calculate somehow the number of meshes before the image
    int meshNumber = 0, imageNumber = 0;
    for (int i = 0; i < d->view->layerCount() + d->view->meshLayerCount(); i++)
        {
            if(d->view->dataInList(i) && d->view->dataInList(i)->identifier().contains("vtkDataMesh"))
            {
                meshNumber++;
            }
            else if(d->view->dataInList(i))
            {
                imageNumber++;
            }
            if(imageNumber - 1 == d->currentLayer )
                break;
        }
    d->view->removeOverlay(meshNumber + imageNumber - 1);
    //d->view->removeOverlay(d->currentLayer);

    d->view->update();

    //remove item if it was the last item: this should never happen since
    //we never delete the layer 0... BEN: I removed the test, should never be needed
//    if (d->currentLayer != 0)
    {
        bool reexpand = d->propertiesTree->selectedItems()[0]->isExpanded();

        d->propertiesTree->invisibleRootItem()->removeChild(d->propertiesTree->selectedItems()[0]);

        //select a new item if the currentLayer was expanded
        if (reexpand)
        {
//            qDebug()<<"let's select item:"<< 0;
            QTreeWidgetItem * newlySelectedItem = d->propertiesTree->topLevelItem(0);
            newlySelectedItem->setExpanded(true);
        }
    }
    d->propertiesTree->clearSelection();

    raiseSlider(d->view->layerCount() == 2);
}


void medViewPropertiesToolBox::raiseSlider(bool isVisible,double opacity)
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

void medViewPropertiesToolBox::onSwitchLayersButtonClicked()
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

QIcon medViewPropertiesToolBox::createIcon(QString colorName)
{
    QPixmap iconPixmap(32,32);
    iconPixmap.fill(QColor(colorName));
    QIcon itemIcon(iconPixmap);
    return itemIcon;
}

void medViewPropertiesToolBox::onWindowingChanged(bool checked)
{
    if (checked && d->view) {
        d->view->setProperty("MouseInteraction", "Windowing");
    }
}

void medViewPropertiesToolBox::onZoomingChanged(bool checked)
{
    if (checked && d->view) {
        d->view->setProperty("MouseInteraction", "Zooming");
    }
}

void medViewPropertiesToolBox::onSlicingChanged(bool checked)
{
    if (checked && d->view) {
        d->view->setProperty("MouseInteraction", "Slicing");
    }
}

void medViewPropertiesToolBox::onMeasuringChanged(bool checked)
{
    if (checked && d->view) {
        d->view->setProperty("MouseInteraction", "Measuring");
    }
}


void medViewPropertiesToolBox::onAxisVisibilityChanged(bool visible)
{
    if (d->view) {
        d->view->setProperty("ShowAxis", (visible ? "true" : "false"));
        d->view->update();
    }
}

void medViewPropertiesToolBox::onScalarBarVisibilityChanged(bool visible)
{
    if (d->view) {
        d->view->setProperty("ShowScalarBar", (visible ? "true" : "false"));
        d->view->update();
    }
}

void medViewPropertiesToolBox::onRulerVisibilityChanged(bool visible)
{
    if (d->view) {
        d->view->setProperty("ShowRuler", (visible ? "true" : "false"));
        d->view->update();
    }
}

void medViewPropertiesToolBox::onAnnotationsVisibilityChanged(bool visible)
{
    if (d->view) {
        d->view->setProperty("ShowAnnotations", (visible ? "true" : "false"));
        d->view->update();
    }
}

void medViewPropertiesToolBox::on2DTriggered(dtkAbstractView* view)
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

void medViewPropertiesToolBox::on3DTriggered(dtkAbstractView* view)
{
    if(view->property("Orientation")=="3D")
    {
        d->propertiesView->hide();
        //d->propView->hide();
        d->view3dToolBoxWidget->show();
    }
}

void medViewPropertiesToolBox::onModeChanged(QString mode)
{
    if (d->view) {
        d->view->blockSignals (true);
        d->view->setProperty("3DMode", mode);
        d->view->blockSignals (false);
        d->view->update();
    }
}

void medViewPropertiesToolBox::onVRModeChanged(QString mode)
{
    if (d->view) {
        d->view->blockSignals (true);
        d->view->setProperty("Renderer", mode);
        d->view->blockSignals (false);
        d->view->update();
    }
}

void medViewPropertiesToolBox::onLodChanged(int value)
{
    if (d->view) {
        d->view->blockSignals (true);
        d->view->setMetaData("LOD", QString::number(value));
        d->view->blockSignals (false);
        d->view->update();
    }
}

void medViewPropertiesToolBox::onCroppingChanged(bool checked)
{
    if (d->view) {
        d->view->blockSignals (true);
        d->view->setProperty("Cropping", (checked ? "true" : "false"));
        d->view->blockSignals (false);
        d->view->update();
    }
}

void medViewPropertiesToolBox::onDepthPeelingChanged(bool checked)
{
    if (d->view) {
        d->view->blockSignals (true);
        d->view->setProperty("DepthPeeling", (checked ? "true" : "false"));
        d->view->blockSignals (false);
        d->view->update();
    }
}

void  medViewPropertiesToolBox::setCurrentInteractionFromSettings()
{
    medSettingsManager * mnger = medSettingsManager::instance();
    QString interaction = mnger->value("interactions","mouse",
                                       "Windowing").toString();
    if (interaction == "Zooming")
    {
        d->zoomingPushButton->setChecked(true);
    }
    else if (interaction == "Slicing")
    {
        d->slicingPushButton->setChecked(true);
    }
    else if (interaction == "Measuring")
    {
        d->measuringPushButton->setChecked(true);
    }
    else
    {
        d->windowingPushButton->setChecked(true);
    }
}

/**
 * Changes the orientation property of the view to Axial if checked is true.
 *
 * @param checked
 */
void medViewPropertiesToolBox::onAxialChanged(bool checked)
{
	if (checked && d->view){
		d->view->setProperty ( "Orientation", "Axial" );
		d->view->update();
		on2DTriggered(d->view);
	}
}
/**
 * Changes the orientation property of the view to Sagittal if checked is true.
 *
 * @param checked
 */
void medViewPropertiesToolBox::onSagittalChanged(bool checked)
{
	if (checked && d->view){
		d->view->setProperty ( "Orientation", "Sagittal" );
		d->view->update();
		on2DTriggered(d->view);
	}
}
/**
 * Changes the orientation property of the view to Coronal if checked is true.
 *
 * @param checked
 */
void medViewPropertiesToolBox::onCoronalChanged(bool checked)
{
	if (checked && d->view){
		d->view->setProperty ( "Orientation", "Coronal" );
		d->view->update();
		on2DTriggered(d->view);
	}
}
/**
 * Changes the view to the 3Dmode if checked is true.
 *
 * @param checked
 */
void medViewPropertiesToolBox::onView3DChanged(bool checked)
{
	if (checked && d->view){
		d->view->setProperty ( "3DMode", d->view->property ( "3DMode" ) );
		d->view->setProperty ( "Orientation", "3D" );
		d->view->update();
		on3DTriggered(d->view);
	}
}
