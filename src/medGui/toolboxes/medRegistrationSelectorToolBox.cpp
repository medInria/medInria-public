/**
 * This class defines the registration toolbox and manages the changes in the different views.
 * It also manages the saving of the transformation and registered image.
 */

#include "medRegistrationSelectorToolBox.h"

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractViewInteractor.h>
#include <dtkCore/dtkSmartPointer.h>

#include <medDataManager.h>
#include <medViewManager.h>
#include <medMessageController.h>
#include <medMetaDataKeys.h>
#include <medAbstractView.h>

#include <medAbstractDataImage.h>

#include <medToolBoxTab.h>
#include <medToolBoxFactory.h>

#include <medRegistrationAbstractToolBox.h>


#include <QtGui>

class medRegistrationSelectorToolBoxPrivate
{
public:
    QPushButton * saveImageButton;
    QPushButton * saveTransButton;

    QComboBox *toolboxes;
    dtkSmartPointer<medAbstractView> fixedView;
    dtkSmartPointer<medAbstractView> movingView;
    dtkSmartPointer<medAbstractView> fuseView;

    dtkSmartPointer<medAbstractDataImage> fixedData;
    dtkSmartPointer<medAbstractDataImage> movingData;

    dtkSmartPointer<dtkAbstractProcess> process;

    medRegistrationAbstractToolBox * customToolBox;
};

medRegistrationSelectorToolBox::medRegistrationSelectorToolBox(QWidget *parent) : medToolBox(parent), d(new medRegistrationSelectorToolBoxPrivate)
{
    d->fuseView = 0;
    d->fixedData  = NULL;
    d->movingData = NULL;
    d->fixedView  = NULL;
    d->movingView = NULL;
    d->process = NULL;

    // Process section
    d->saveImageButton = new QPushButton(tr("Export Image"),this);
    d->saveImageButton->setToolTip(tr("Save registered image to the File System"));
    connect (d->saveImageButton, SIGNAL(clicked()), this, SLOT(onSaveImage()));

    d->saveTransButton = new QPushButton(tr("Export Transf."),this);
    d->saveTransButton->setToolTip(
                tr("Export the resulting transformation to the File System"));
    connect (d->saveTransButton, SIGNAL(clicked()), this, SLOT(onSaveTrans()));


    // --- Setting up custom toolboxes list ---

    d->toolboxes = new QComboBox(this);
    d->toolboxes->addItem(tr("Choose algorithm"));
    d->toolboxes->setToolTip(
                tr( "Choose the registration algorithm"
                    " amongst the loaded plugins" ));
    d->toolboxes->setStyleSheet("QComboBox{margin-top: 5px}");
    medToolBoxFactory* tbFactory =medToolBoxFactory::instance();
    int i=1;
    foreach(QString toolbox, tbFactory->toolBoxesFromCategory("registration"))
    {
        medToolBoxDetails* details = tbFactory->toolBoxDetailsFromId(toolbox);
//        qDebug() << "Added registration toolbox" << name;
        d->toolboxes->addItem(details->name, toolbox);
        d->toolboxes->setItemData(i,
                                  details->description,
                                  Qt::ToolTipRole);
        i++;
    }

    connect(d->toolboxes, SIGNAL(activated(int)), this, SLOT(onToolBoxChosen(int)));


    // /////////////////////////////////////////////////////////////////
    // Setup
    // /////////////////////////////////////////////////////////////////


    // ---
    QButtonGroup *layoutButtonGroup = new QButtonGroup(this);
    layoutButtonGroup->addButton(d->saveImageButton);
    layoutButtonGroup->addButton(d->saveTransButton);

    QHBoxLayout *layoutButtonLayout = new QHBoxLayout;
    layoutButtonLayout->addWidget(d->saveImageButton);
    layoutButtonLayout->addWidget(d->saveTransButton);

    QVBoxLayout *layoutLayout = new QVBoxLayout;
    layoutLayout->addLayout(layoutButtonLayout);

    QWidget * layoutSection = new QWidget(this);
    layoutSection->setLayout(layoutLayout);


    addWidget(d->toolboxes);
    addWidget(layoutSection);


    this->setTitle(tr("Registration"));
    d->customToolBox = NULL;

    //Connect Message Controller:
    connect(this,SIGNAL(showError(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showError(const QString&,unsigned int)));
    connect(this,SIGNAL(showInfo(const QString&,unsigned int)),
            medMessageController::instance(),SLOT(showInfo(const QString&,unsigned int)));
}

medRegistrationSelectorToolBox::~medRegistrationSelectorToolBox(void)
{
    delete d;

    d = NULL;
}

//! Gets the fixedView.
dtkAbstractView *medRegistrationSelectorToolBox::fixedView(void)
{
    return d->fixedView;
}

//! Gets the movingView.
dtkAbstractView *medRegistrationSelectorToolBox::movingView(void)
{
    return d->movingView;
}

//! Gets the fuseView.
dtkAbstractView *medRegistrationSelectorToolBox::fuseView(void)
{
    return d->fuseView;
}

//! Gets the fixedData.
medAbstractDataImage *medRegistrationSelectorToolBox::fixedData(void)
{
    return d->fixedData;
}

//! Gets the movingData.
medAbstractDataImage *medRegistrationSelectorToolBox::movingData(void)
{
    return d->movingData;
}

/** 
 * Sets up the fixedView and the layer 0 of the fuseView.
 *
 * @param index The index that was imported.
 */
void medRegistrationSelectorToolBox::onFixedImageDropped (const medDataIndex& index)
{

    if (!index.isValid())
        return;

    d->fixedData = medDataManager::instance()->data(index);

    if (!d->fixedData)
        return;

    d->fixedView = dynamic_cast<medAbstractView*>
                   (medViewManager::instance()->views(index).first());

    if(!d->fixedView) {
        qDebug() << "Unable to retrieve fixed view";
	return;
    }

    if (d->fuseView)
    {
        d->fuseView->blockSignals(true);
        if (d->movingView && d->fuseView->layerCount()==1)
        {
            //only the moving view has been set: shift it to layer 1
            d->fuseView->setData(d->fixedData,0);
            d->fuseView->setData(d->movingData,1);
        }
        else
        {
            //either both views are set, or only the fixed view
            d->fuseView->setData(d->fixedData,0);
        }
        d->fuseView->reset();
        d->fuseView->update();
        this->synchroniseWindowLevel(d->movingView); // This line will synchronise the windowlvl between the movingView and fuseView.
        this->synchroniseWindowLevel(d->fixedView); // This line will synchronise the windowlvl between the fixedView and fuseView.
        d->fuseView->blockSignals(false);
    }
    connect(d->fixedView,SIGNAL(positionChanged(QVector3D,bool)),this,SLOT(synchronisePosition(QVector3D)));
    connect(d->fixedView,SIGNAL(windowingChanged(double,double,bool)),this,SLOT(synchroniseWindowLevel(void)));
    if (!d->movingView)
    {
        connect(d->fuseView,SIGNAL(positionChanged(QVector3D,bool)),this,SLOT(synchronisePosition(QVector3D)));
        connect(d->fuseView,SIGNAL(windowingChanged(double,double,bool)),this,SLOT(synchroniseWindowLevel(void)));
    }
}

/** 
 * Sets up the movingView and the layer 1 of the fuseView.
 *
 * @param index The index that was imported.
 */
void medRegistrationSelectorToolBox::onMovingImageDropped (const medDataIndex& index)
{

    if (!index.isValid())
        return;

    d->movingData = medDataManager::instance()->data(index);

    if (!d->movingData)
        return;

    d->movingView = dynamic_cast<medAbstractView*>
                    (medViewManager::instance()->views
                     (index).first());

    if(!d->movingView) {
        qDebug() << "Unable to retrieve moving view";
	return;
    }

    if (d->fixedView) {
        d->movingView->update();
    }

    d->fuseView->blockSignals(true);
    if (d->fixedView)
    {
        //already one layer present
        d->fuseView->setData(d->movingData,1);
    }
    else
    {
        //only the moving view is set
        d->fuseView->setData(d->movingData,0);
    }
    d->fuseView->reset();
    d->fuseView->update();
    this->synchroniseWindowLevel(d->movingView); // This line will synchronise the windowlvl between the movingView and fuseView.
    this->synchroniseWindowLevel(d->fixedView); // This line will synchronise the windowlvl between the fixedView and fuseView.
    d->fuseView->blockSignals(false);

    
    connect(d->movingView,SIGNAL(positionChanged(QVector3D,bool)),this,SLOT(synchronisePosition(QVector3D)));
    connect(d->movingView,SIGNAL(windowingChanged(double,double,bool)),this,SLOT(synchroniseWindowLevel(void)));
    if (!d->fixedView)
    {
        connect(d->fuseView,SIGNAL(positionChanged(QVector3D,bool)),this,SLOT(synchronisePosition(QVector3D)));
        connect(d->fuseView,SIGNAL(windowingChanged(double,double,bool)),this,SLOT(synchroniseWindowLevel(void)));
    }
}

/** 
 * Sets up the toolbox chosen and remove the old one.
 *
 * @param index The index of the toolbox that was chosen.
 */
void medRegistrationSelectorToolBox::onToolBoxChosen(int index)
{
    //get identifier for toolbox.
    QString id = d->toolboxes->itemData(index).toString();

    medRegistrationAbstractToolBox *toolbox = qobject_cast<medRegistrationAbstractToolBox*>(medToolBoxFactory::instance()->createToolBox(id));


    if(!toolbox) {
        qWarning() << "Unable to instantiate" << id << "toolbox";
        return;
    }

    toolbox->setRegistrationToolBox(this);
    //get rid of old toolBox
    if (d->customToolBox)
    {
        emit removeToolBox(d->customToolBox);
        delete d->customToolBox;
    }
    d->customToolBox = toolbox;
    toolbox->show();
    emit addToolBox(toolbox);

    connect (toolbox, SIGNAL (success()), this, SLOT (onSuccess()));
    connect (toolbox, SIGNAL (failure()), this, SIGNAL (failure()));
}

/** 
 * Sets the fuseView.
 *
 * @param view The new fuseView.
 */
void medRegistrationSelectorToolBox::setFuseView(dtkAbstractView *view)
{
    if (!view)
        return;

    d->fuseView = dynamic_cast <medAbstractView*> (view);
}

//! Clears the toolbox.
void medRegistrationSelectorToolBox::clear(void)
{

    //maybe clear the customtoolbox?
    if (d->customToolBox)
        d->customToolBox->clear();
}

//! Gets the process.
dtkAbstractProcess * medRegistrationSelectorToolBox::process(void)
{
    return d->process;
}

/** 
 * Sets the process.
 *
 * @param proc The new process.
 */
void medRegistrationSelectorToolBox::setProcess(dtkAbstractProcess* proc)
{
    d->process = proc;
}

//! Saves the registered image.
void medRegistrationSelectorToolBox::onSaveImage()
{
    if ( !d->movingData)
    {
        emit showError(tr  ("Please Select a moving image before saving"),3000);
        return;
    }
    if (!d->process )
    {
        emit showError(tr  ("Please run the registration before saving"),3000);
        return;
    }
    QFileDialog dialog(this, tr("Save Image"),
                               QDir::homePath(),
                               tr("MetaFile (*.mha *.mhd);;Nifty (*.nii);;Analyse (*.hdr);;Nrrd (*.nrrd);;VTK (*.vtk);;All supported files (*.mha *.mhd *.nii *.hdr *.nrrd)"));
    dialog.setDefaultSuffix("mha");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList fileName;
    if (dialog.exec())
        fileName = dialog.selectedFiles();

    qDebug() << fileName;

    if (!fileName.isEmpty())
    {
        //qDebug()<< (void *) d->movingData;
        //qDebug()<<  d->movingView->data();
        if (d->process->write(fileName[0]))
        {
            emit(showInfo(tr  ("Registered Image Saved"),3000));
        }
        else
        {
            emit(showError(tr  ("Image saving failed, no suitable writer found"),3000));
        }
    }

}

//! Saves the transformation.
void medRegistrationSelectorToolBox::onSaveTrans()
{
    if (!d->movingData)
    {
        emit showError(tr  ("Please Select a moving image before saving"),3000);
        return;
    }
    if (!d->process )
    {
        emit showError(tr  ("Please run the registration before saving"),3000);
        return;
    }

    //get the transformation type: affine or deformation field.
    QString fileTypeSuggestion;
    QString defaultSuffix;
    if (d->process->hasProperty("transformType"))
    {
        if ( d->process->property("transformType") == "rigid")
        {
            fileTypeSuggestion = tr("Transformation (*.txt)");
            defaultSuffix = "txt";
        }
        else
        {
            defaultSuffix = "mha";
            fileTypeSuggestion = tr("MetaFile (*.mha *.mhd);;Nifty (*.nii);;"
                                    "Analyse (*.hdr);;Nrrd (*.nrrd);;"
                                    "VTK (*.vtk);;"
                                    "All supported files "
                                    "(*.mha *.mhd *.nii *.hdr *.nrrd)");
        }
    }

    QFileDialog dialog(this, tr("Save Transformation"),
                               QDir::homePath(),
                               fileTypeSuggestion);

    dialog.setDefaultSuffix("mha");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList fileName;
    if (dialog.exec())
        fileName = dialog.selectedFiles();

    qDebug() << fileName;

    if (!fileName.isEmpty())
    {
        //qDebug()<< (void *) d->movingData;
        //qDebug()<<  d->movingView->data();
        QStringList transformFileName;
        transformFileName << ""<< fileName[0];
        if (d->process->write(transformFileName))
        {
            emit(showInfo(tr  ("Transformation saved"),3000));
        }
        else
        {
            emit(showError(tr  ("Transformation saving failed, no suitable writer found"),3000));
        }
    }
}

//! If the registration has ended well, it sets the output's metaData and reset the movingView and fuseView with the registered image.
void medRegistrationSelectorToolBox::onSuccess()
{
    dtkSmartPointer<dtkAbstractData> output(d->process->output());

    foreach(QString metaData, d->fixedData->metaDataList())
        output->addMetaData(metaData,d->fixedData->metaDataValues(metaData));

    foreach(QString property, d->fixedData->propertyList())
        output->addProperty(property,d->fixedData->propertyValues(property));

    QString newDescription = d->movingData->metadata(medMetaDataKeys::SeriesDescription.key());
    newDescription += " registered";
    output->setMetaData(medMetaDataKeys::SeriesDescription.key(), newDescription);

    QString generatedID = QUuid::createUuid().toString().replace("{","").replace("}","");
    output->setMetaData ( medMetaDataKeys::SeriesID.key(), generatedID );

    medDataManager::instance()->importNonPersistent(output);

    if(output)
    {
        d->movingView->setData(output,0);
        // calling reset() will reset all the view parameters (position - zoom - window/level) to default
        d->movingView->reset();
        d->movingView->update();
        d->fuseView->setData(output,1);
        d->fuseView->update();
        d->fixedView->setLinkPosition(true);
        d->fixedView->setLinkCamera(true);
        d->movingView->setLinkPosition(true);
        d->movingView->setLinkCamera(true);
    }
}

//! Synchronises the window/level of the layer 0 of the fixedView with the layer 0 of the fuseView, and the layer 0 of the movingView with the layer 1 of the fuseView. 
void medRegistrationSelectorToolBox::synchroniseWindowLevel(QObject * sender){ 
			
    double window,level;
    QObject * senderView;
    if (sender)
        senderView = sender;
    else
        senderView = QObject::sender();

    if (d->fixedView==senderView)
    {
        d->fixedView->windowLevel(level,window);
        d->fuseView->setCurrentLayer(0);
        d->fuseView->onWindowingChanged(level,window);
    }
    else if (d->movingView==senderView)
    {
        d->movingView->windowLevel(level,window);
        d->fuseView->setCurrentLayer(1);
        d->fuseView->onWindowingChanged(level,window);
    }
    else
    {		
        d->fuseView->windowLevel(level,window);
        if (d->fixedView && d->fixedView->windowingLinked() && d->movingView && d->movingView->windowingLinked())
        {
            d->fixedView->onWindowingChanged(level,window);
            d->movingView->onWindowingChanged(level,window);
            if (d->fuseView->currentLayer()==0)// Since the fixed view and moving view are linked we must assure that the two layers of the fuse view are changed.
            { 
                d->fuseView->setCurrentLayer(1);
                d->fuseView->onWindowingChanged(level,window);
            }
            else if (d->fuseView->currentLayer()==1)
            {
                d->fuseView->setCurrentLayer(0);
                d->fuseView->onWindowingChanged(level,window);
            }
        }
        else if (d->fixedView && d->fuseView->currentLayer()==0)
            d->fixedView->onWindowingChanged(level,window);
        else if (d->movingView && (d->fuseView->currentLayer()==1 || d->fuseView->layerCount()==1))	
            d->movingView->onWindowingChanged(level,window);
        // In the case that the currentLayer>1 we do nothing.
    }
}

//! Synchronises the position between the compare and the fuse mode.
void medRegistrationSelectorToolBox::synchronisePosition(const QVector3D &position){ 
   
    if (d->fixedView==QObject::sender() || d->movingView==QObject::sender())
    {
        if (d->fixedView && d->fixedView->positionLinked() && d->movingView &&  d->movingView->positionLinked()) // If the fixedView and movingView are linked in position then the changes also appear in fuseView.
            d->fuseView->onPositionChanged(position);
    }
    else // the changes in fuseView are propagated to the fixedView and movingView.
    {		
        if (d->fixedView)
            d->fixedView->onPositionChanged(position);
        if (d->movingView)
            d->movingView->onPositionChanged(position);
    }
}

