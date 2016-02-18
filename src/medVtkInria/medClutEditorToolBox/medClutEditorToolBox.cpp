/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#include "medClutEditorToolBox.h"
#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <dtkCore/dtkAbstractView.h>
#include <medAbstractImageView.h>
#include <medStorage.h>
#include <medToolBoxFactory.h>
#include <medToolBoxHeader.h>
#include <medViewContainer.h>
#include <medTabbedViewContainers.h>
#include <medVtkViewBackend.h>

#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkLookupTable.h>
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
#include <qsizepolicy.h>

#include <math.h>
#include <limits>

#include <medLUTToXMLWriter.h>
#include <medXMLToLUTReader.h>
#include <medSaveLUTDialog.h>
#include <medLoadLUTDialog.h>

// /////////////////////////////////////////////////////////////////
// medClutEditorToolBox
// /////////////////////////////////////////////////////////////////

class medClutEditorToolBoxPrivate
{
public:
    QAction *newAction;
    // QAction *deleteAction; // problem: right click deselects vertices
    // QAction *colorAction;  // problem: right click deselects vertices
    QAction *applyAction;
    QAction *loadTableAction;
    QAction *saveTableAction;
    // QAction *deleteTableAction;
    QAction *toggleDirectUpdateAction;
    QAction *discreteModeAction;
    QAction *infoAction;
    medClutEditorScene *scene;
    medClutEditorView  *view;
    medClutEditorHistogram *histogram;
    unsigned int layerForced;
    bool discreteMode;
    bool invertLUT; //for the moment, only used in substrateWorkspace

    medAbstractData *dtk_data;
    medAbstractView *med_view;
    QList <medClutEditorTable *> * tables;
};

medClutEditorToolBox::medClutEditorToolBox(QWidget *parent) : medToolBox(parent)
{
    d = new medClutEditorToolBoxPrivate;
    d->scene = new medClutEditorScene;
    d->view  = new medClutEditorView( this );
    d->view->setScene(d->scene);
    d->histogram = NULL;
    d->med_view = NULL;

    d->newAction                = new QAction("New table",    this);
    d->loadTableAction          = new QAction("Load table",   this);
    d->saveTableAction          = new QAction("Save table",   this);
    // d->deleteTableAction        = new QAction("Delete table", this);
    // d->colorAction              = new QAction("Edit color", this);
    // d->deleteAction             = new QAction("Delete selection", this);
    d->applyAction              = new QAction("Apply", this);
    d->toggleDirectUpdateAction = new QAction("Direct update", this);
    d->discreteModeAction = new QAction("Discrete Mode", this);
    d->infoAction = new QAction(QPixmap(":icons/information.png"), tr("About"), this);

    d->loadTableAction->setEnabled( false );
    d->saveTableAction->setEnabled( false );
    d->toggleDirectUpdateAction->setCheckable( true );
    d->toggleDirectUpdateAction->setChecked( true );
    d->discreteModeAction->setCheckable( true );

    connect(d->newAction,                SIGNAL(triggered()),
            this,                        SLOT(onNewTableAction()));
    connect(d->loadTableAction,          SIGNAL(triggered()),
            this,                        SLOT(onLoadTableAction()));
    connect(d->saveTableAction,          SIGNAL(triggered()),
            this,                        SLOT(onSaveTableAction()));
    // connect(d->deleteTableAction,        SIGNAL(triggered()),
    //         this,                        SLOT(onDeleteTableAction()));
    // connect(d->colorAction,              SIGNAL(triggered()),
    //         this,                        SLOT(onColorAction()));
    // connect(d->deleteAction,             SIGNAL(triggered()),
    //         this,                        SLOT(onDeleteAction()));
    connect(d->applyAction,              SIGNAL(triggered()),
            this,                        SLOT(onApplyTablesAction()));
    connect(d->toggleDirectUpdateAction, SIGNAL(triggered()),
            this,                        SLOT(onToggleDirectUpdateAction()));
    connect(d->discreteModeAction,       SIGNAL(toggled(bool)),
            this,                        SLOT(setDiscreteMode(bool)));
    connect(d->infoAction,               SIGNAL(triggered()),
            this,                        SLOT(showInfo()));

    d->discreteModeAction->setChecked( false );


    QWidget* widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout;
    //layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->view);
    widget->setLayout(layout);
    this->addWidget(widget);
    
    this->setTitle("CLUT Editor");
    widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    
    //Disables minimizing
    disconnect(this->header(),SIGNAL(triggered()),this,SLOT(switchMinimize()));

    //get saved LUTs.
    d->tables = new QList<medClutEditorTable *>();
    medXMLToLUTReader reader = medXMLToLUTReader(d->tables);
    QString lutFileName = medStorage::dataLocation();
    lutFileName.append("/LUTs.xml");
    QFile file(lutFileName);
    if (file.open(QIODevice::ReadOnly)) {
        reader.read(&file);
        file.close();
    }

    d->discreteMode = false;
    d->invertLUT = false;  //"conventional" LUT is /
    d->layerForced = 0;
}

medClutEditorToolBox::~medClutEditorToolBox(void)
{
    delete d->newAction;
    delete d->loadTableAction;
    delete d->saveTableAction;
    // delete d->deleteTableAction;
    // delete d->colorAction;
    // delete d->deleteAction;
    delete d->applyAction;
    delete d->toggleDirectUpdateAction;
    delete d->discreteModeAction;
    delete d->infoAction;

    delete d->scene;
    delete d->view;
    delete d->tables; //TODO: delete all the tables as well.
    delete d;
}

void medClutEditorToolBox::reset()
{
    d->dtk_data = NULL;
    d->med_view = NULL;
    d->discreteMode = false;
    d->layerForced = 0;
}

medClutEditorScene * medClutEditorToolBox::getScene()
{
    return d->scene;
}

bool medClutEditorToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medClutEditorToolBox>();
}

void medClutEditorToolBox::setData(medAbstractData *data)
{
    if ( data == NULL ) {
        this->deleteTable();
        d->dtk_data = data;
    }

    if ( data == d->dtk_data )
        return;

    if (medAbstractImageData *image =
        dynamic_cast<medAbstractImageData *>(data)) {

        if ( d->histogram != NULL )
            delete d->histogram;

        d->histogram = new medClutEditorHistogram();
        d->scene->addItem( d->histogram );
        int min_range = image->minRangeValue();
        int max_range = image->maxRangeValue();
        QMap<qreal, qreal> bins;

        for ( int i = min_range; i <= max_range; ++i ) {
            qreal count = static_cast< qreal >(
                image->scalarValueCount( i - min_range ) ); //the histogram (calculated in itkDataImage)'s first value is 0
                                                            //otherwise shift of this histogram
            bins.insert( static_cast< qreal >( i ), count );
        }
        d->histogram->setValues( bins );

        this->initializeTable();

        d->dtk_data = image;
    }
}

void medClutEditorToolBox::setView(medAbstractView *view)
{
    if(!view)
        return;

    medAbstractImageData * image;

    if(medAbstractImageView *imageView = qobject_cast<medAbstractImageView*>(view))
        image = static_cast<medAbstractImageData *>(imageView->layerData(imageView->currentLayer()));
    else
        image = static_cast<medAbstractImageData*>(view->dtkAbstractView::data());

    if( !image || image->identifier()=="vtkDataMesh" ) //no windowing on meshes
        return;

    this->setData(image);

    if (view == d->med_view)
        return;

    d->med_view = view;

    if (d->med_view != NULL)
    {
        QList<double> scalars;
        QList<QColor> colors;

        // Allows to modif medClutTbx when windowing with dragging on the view
        // AND used to initialize tbx with the image LUT
        this->getTransferFunctions( scalars, colors );
        medClutEditorTable * table = d->scene->table();
        table->setTransferFunction( scalars, colors );
    }
}

void medClutEditorToolBox::initializeTable(void)
{
    this->deleteTable();

    medClutEditorTable *lut = new medClutEditorTable("Unknown");
    d->scene->addItem( lut );
    connect (lut, SIGNAL(vertexMoving()), this, SLOT(onVertexMoved()));
    connect (lut, SIGNAL(vertexSet()), this, SLOT(onVertexMoved()));
    connect (lut, SIGNAL(vertexAdded()),   this, SLOT(onVertexMoved()));
    connect (lut, SIGNAL(vertexRemoved()), this, SLOT(onVertexMoved()));
    if(d->discreteMode)
    {
        lut->setDiscreteMode(true);
        d->discreteModeAction->setChecked(true); //update GUI
    }
}

void medClutEditorToolBox::deleteTable(void)
{
    medClutEditorTable * table;
    if(!d->scene->table())
        return;
    while ( (table = d->scene->table()) ) {
        d->scene->removeItem( table );
        delete table;
    }
}

void medClutEditorToolBox::applyTable(medAbstractView* view)
{
    if ( view != NULL )
    {
        QList<double> scalars;
        QList<QColor> colors;
        medClutEditorTable * clutTable = d->scene->table();
        clutTable->getTransferFunction(scalars, colors);
        this->setColorLookupTable(view, scalars, colors);
        view->viewWidget()->update();

        //modifies the interpolation used before rendering (default=linear, set when setting VR), 
        //Neareast-N does not create new values (good for histogram consistency)
        static_cast<medVtkViewBackend*>(view->backend())->view3D->GetVolumeProperty()->SetInterpolationTypeToNearest();
    }
}


void medClutEditorToolBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        event->accept();

        QMenu menu("Color Lookup Table", this);
        menu.setWindowOpacity(0.8);
        if  ( !d->toggleDirectUpdateAction->isChecked() )
            menu.addAction(d->applyAction);
        menu.addAction(d->toggleDirectUpdateAction);
        menu.addAction(d->discreteModeAction);
        menu.addSeparator();
        menu.addAction(d->newAction);
        menu.addAction(d->loadTableAction);
        menu.addAction(d->saveTableAction);
        menu.addAction(d->infoAction);
        // menu.addAction(d->deleteTableAction);
        // menu.addSeparator();
        // menu.addAction(d->colorAction);
        // menu.addAction(d->deleteAction);
        menu.exec(mapFrom(this, QCursor::pos()));
        //hack to get the rubber band back after right clicking on the view
        d->view->setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void medClutEditorToolBox::onNewTableAction(void)
{
    this->initializeTable();
    if ( medClutEditorTable * table = d->scene->table() )
        table->triggerVertexSet();
}

// void medClutEditorToolBox::onColorAction(void)
// {
//     if ( medClutEditorTable * table = d->scene->table() )
//         table->setColorOfSelection();
// }

// void medClutEditorToolBox::onDeleteAction(void)
// {
//     if ( medClutEditorTable * table = d->scene->table() )
//         table->deleteSelection();
// }

void medClutEditorToolBox::onVertexMoved(void)
{
    if ( d->scene->table() )
    {
        if ( d->toggleDirectUpdateAction->isChecked() )
            this->applyTable(d->med_view);
    }
}

void medClutEditorToolBox::onApplyTablesAction(void)
{
    this->applyTable(d->med_view);
}

void medClutEditorToolBox::onToggleDirectUpdateAction(void)
{
    if  ( d->toggleDirectUpdateAction->isChecked() )
        this->applyTable(d->med_view);
}

void medClutEditorToolBox::setDiscreteMode(bool value)
{
    d->discreteMode = value;
    if(this->getScene()->table())
        this->getScene()->table()->setDiscreteMode(value);  //purely visual changes
    this->applyTable(d->med_view);                          // LUT changes
}

void medClutEditorToolBox::invertLUT(bool value)
{
    d->invertLUT = value;
    if(this->getScene()->table())
        this->getScene()->table()->invertLUT(value);
    this->applyTable(d->med_view);
}


void medClutEditorToolBox::onLoadTableAction(void)
{
    QStringList titles;
    foreach (medClutEditorTable * table,*d->tables ) {
        titles << table->title();
    }

    medLoadLUTDialog dialog (titles,this);
    // Show it and wait for Ok or Cancel
    if( dialog.exec() == QDialog::Accepted ) {
        // We got Ok
        foreach (medClutEditorTable * table,*d->tables ) {
            if ( table->title() == dialog.textValue()) {
                //display this table
                d->scene->removeItem(d->scene->table());
                d->scene->addItem(table);
                d->scene->update();
            }
        }
    }

}

void medClutEditorToolBox::onSaveTableAction(void)
{

    if ( medClutEditorTable * table = d->scene->table() ) {
        medSaveLUTDialog dialog (table->title(),this);
        // Show it and wait for Ok or Cancel
        if( dialog.exec() == QDialog::Accepted ) {
            // We got Ok
            medClutEditorTable * tableToSave;
            if (table->title().compare(dialog.textValue())) {
                //copy the table
                tableToSave = new medClutEditorTable( * table );
                tableToSave->setTitle(dialog.textValue());
            }
            else {
                tableToSave = table;
            }

            QList<medClutEditorTable*>  l = *d->tables;
            l << tableToSave;
            medLUTToXMLWriter writer (l);
            //create file object in an existing storage place
            medStorage::mkpath(medStorage::dataLocation());
            QString fileName = medStorage::dataLocation();
            fileName.append("/LUTs.xml");
            QFile * file = new QFile();
            file->setFileName(fileName);
            if (file->open(QIODevice::WriteOnly)) {
                writer.writeFile(file);
                file->close();
            }
            else {
                qDebug() << "can't open file " << fileName;
            }
            delete file;
        }
    }
}

// void medClutEditorToolBox::onDeleteTableAction(void)
// {

// }


void medClutEditorToolBox::clear()
{
    medTabbedViewContainers * containers = this->getWorkspace()->stackedViewContainers();
    QList<medViewContainer*> containersInTabSelected = containers->containersInTab(containers->currentIndex());
    medAbstractView *view=NULL;
    for(int i=0;i<containersInTabSelected.length();i++)
    {
        if (containersInTabSelected[i]->isSelected())
        {
            view = containersInTabSelected[i]->view();
            break;
        }
    }

    this->setView(qobject_cast<medAbstractImageView*>(view));
}


void medClutEditorToolBox::getTransferFunctions ( QList<double> & scalars,
                                     QList<QColor> & colors )
{
    vtkColorTransferFunction * color   =
        static_cast<medVtkViewBackend*>(d->med_view->backend())->view2D->GetColorTransferFunction(d->layerForced);
    vtkPiecewiseFunction     * opacity =
        static_cast<medVtkViewBackend*>(d->med_view->backend())->view2D->GetOpacityTransferFunction(d->layerForced);

    if(!color || !opacity)
        return;

    if ( color->GetSize() != opacity->GetSize() )
        qWarning() << Q_FUNC_INFO << " sizes of color and opacity transfer "
        "functions don't match!";
    int size = qMin ( color->GetSize(), opacity->GetSize() );

    scalars.clear();
    colors.clear();

    bool ok = true;
    for ( int i = 0; i < size; i++ )
    {
        double xrgb[6], xalpha[4];
        color->GetNodeValue ( i, xrgb );
        opacity->GetNodeValue ( i, xalpha );
        if ( xrgb[0] != xalpha[0] )
            ok = false;

        scalars << xrgb[0];
        QColor c;
        c.setRgbF ( xrgb[1], xrgb[2], xrgb[3], xalpha[1] );
        colors << c;
    }

    if ( !ok )
        qWarning() << Q_FUNC_INFO << " x values of color and opacity transfer "
        "functions don't match!";
}

void medClutEditorToolBox::setColorLookupTable ( medAbstractView *view, QList<double> scalars, QList<QColor> colors )
{
    int size= qMin ( scalars.count(),colors.count() );
    vtkColorTransferFunction * ctf = vtkColorTransferFunction::New();
    //ctf->ClampingOff();
    vtkPiecewiseFunction * pf = vtkPiecewiseFunction::New();
    //pf->ClampingOff();
    for ( int i=0;i<size;i++ )
    {
        ctf->AddRGBPoint ( scalars.at ( i ),
                           colors.at ( i ).redF(),
                           colors.at ( i ).greenF(),
                           colors.at ( i ).blueF(),
                           d->discreteMode ? !d->invertLUT : 0.5,   //midpoint (point between 2 vertices): 
                                                                    //either on the right vertex (1), on the left (0), or at the middle (0.5)
                           d->discreteMode ? 1 : 0.0);  //sharpness (distribution between 2 vertices): either constant or linear
        
        pf->AddPoint (  scalars.at ( i ),
                        colors.at ( i ).alphaF(),
                        d->discreteMode ? !d->invertLUT : 0.5,
                        d->discreteMode ? 1 : 0.0);
    }

    double min = scalars.first();
    double max = scalars.last();
    int n = static_cast< int > ( max - min ) + 1;
    n = std::max(n, size);
    double * table = new double[3*n];
    double * alphaTable = new double[n];

    double * range = ctf->GetRange();
    static_cast<medVtkViewBackend*>(view->backend())->view2D->SetColorRange(range);
    static_cast<medVtkViewBackend*>(view->backend())->view2D->SetColorTransferFunction(ctf);
    static_cast<medVtkViewBackend*>(view->backend())->view2D->SetOpacityTransferFunction(pf);

    //static_cast<medVtkViewBackend*>(view->backend())->view3D->SetColorRange(range);
    //static_cast<medVtkViewBackend*>(view->backend())->view3D->SetTransferFunctions(ctf, pf, d->layerForced);
    
    //static_cast<medVtkViewBackend*>(view->backend())->view3D->vtkImageView::SetColorTransferFunction(ctf);
    //static_cast<medVtkViewBackend*>(view->backend())->view3D->vtkImageView::SetOpacityTransferFunction(pf);

    ctf->GetTable ( min, max, n, table );
    ctf->Build();
    ctf->Delete();
    pf->GetTable ( min,max,n,alphaTable );
    pf->Delete();

    vtkLookupTable * lut = vtkLookupTable::New();
    lut->SetNumberOfTableValues ( n + 2 );
    lut->SetTableRange ( min - 1.0, max + 1.0 );
          
    lut->SetTableValue ( 0, 0.0, 0.0, 0.0, 0.0 );
    int i, j;
    for ( i = 0, j = 0; i < n; ++i, j += 3 )
        lut->SetTableValue ( i+1, table[j], table[j+1], table[j+2], alphaTable[i] );
    //lut->SetTableValue ( n + 1, table[j-3], table[j-2], table[j-1], alphaTable[i-1] ); //last value of the lut
    lut->SetTableValue ( n + 1, 0, 0, 0, 0); //last value of the lut (values above max are black )

    static_cast<medVtkViewBackend*>(view->backend())->view2D->SetLookupTable ( lut );
    unsigned int layer = static_cast<medAbstractLayeredView*>(view)->currentLayer();
    static_cast<medVtkViewBackend*>(view->backend())->view3D->SetLookupTable ( lut, layer );

    lut->Delete();
    delete [] table;
    delete [] alphaTable;
}

void medClutEditorToolBox::addVertex(QPointF value, QColor color)
{
    QPointF position = this->getScene()->valueToCoordinate(value);
    medClutEditorVertex *vertex = new medClutEditorVertex(value, position, color);
    this->getScene()->table()->addVertex(vertex);
    this->getScene()->table()->finalizeMoveSelection();
}

void medClutEditorToolBox::forceLayer(int layer)
{
    d->layerForced = layer;
}

void medClutEditorToolBox::showInfo()
{
    QDialog *messageBox = new QDialog(this);
    messageBox->setWindowTitle(tr("CLUT Editor"));
    QString description = "This histogram allows you to interactively apply a LUT to your image.<br>";
    description += "This LUT can be either discrete or linear.";
    QTextBrowser* descriptionWidget = new QTextBrowser;
    descriptionWidget->setHtml(description);

    QString shortcuts =
        "<b>Double click:</b> Add a new vertex.<br>";
    shortcuts +=" <b>Delete:</b> Remove the selected vertex.<br>";
    shortcuts +=" <b>a:</b> Select all vertices.<br>";
    shortcuts +=" <b>c:</b> Modify color of the selected vertex.<br>";
    shortcuts +=" <b>+/-:</b> Modify transparency of the LUT representation";
    QTextBrowser* shortcutsWidget = new QTextBrowser;
    shortcutsWidget->setHtml(shortcuts);

    QFormLayout *layout = new QFormLayout(messageBox);
    //layout->addWidget(new QLabel(shortcuts));
    layout->addRow("Description", descriptionWidget);
    layout->addRow("Shortcuts", shortcutsWidget);
    messageBox->setLayout(layout);
    //messageBox->exec();
    messageBox->show();
}
