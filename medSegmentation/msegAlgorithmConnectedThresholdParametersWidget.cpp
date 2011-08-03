#include "msegAlgorithmConnectedThresholdParametersWidget.h"
#include "msegController.h"
#include "msegView.h"
#include "msegAlgorithmConnectedThreshold.h"
#include "msegSeedPointAnnotationData.h"

#include <medAbstractViewScene.h>

#include <medCore/medAbstractView.h>
#include <medCore/medAbstractData.h>
#include <medCore/medDataIndex.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkLog.h>
#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkGlobal.h>

#include <QPushButton>
#include <QTableWidget>

#include <algorithm>
#include <set>

namespace mseg {

    static const QString MED_METADATA_PATIENT_NAME = "PatientName";
    static const QString MED_METADATA_STUDY_DESCRIPTION = "StudyDescription";
    static const QString MED_METADATA_SERIES_DESCRIPTION = "SeriesDescription";

    static const QString SEED_POINT_ANNOTATION_DATA_NAME = SeedPointAnnotationData::s_description();

class SingleClickEventFilter : public View 
{
public:
    SingleClickEventFilter( Controller * controller, AlgorithmConnectedThresholdParametersWidget *cb ) : 
        View(controller), 
        m_cb(cb) 
        {}

    virtual bool mousePressEvent( medAbstractViewScene *vscene, QGraphicsSceneMouseEvent *mouseEvent ) MED_OVERRIDE
    {

        Q_ASSERT( vscene );

        medAbstractView * view = vscene->view();

        mouseEvent->accept();

        if (vscene->isScene2D()) {
            // Convert mouse click to a 3D point in the image.

            QVector3D posImage = vscene->sceneToWorld( mouseEvent->scenePos() );
            //Project vector onto plane
            dtkAbstractData * viewData = Controller::viewData( view );
            m_cb->onViewMousePress( view, posImage );

            this->removeFromAllViews();
        }
        return mouseEvent->isAccepted();
    }
private :
    AlgorithmConnectedThresholdParametersWidget *m_cb;

};

AlgorithmConnectedThresholdParametersWidget::AlgorithmConnectedThresholdParametersWidget( Controller *controller, QWidget *parent ) :
    AlgorithmParametersWidget( controller, parent),
    m_viewState(ViewState_None),
    m_noDataText( tr("[No input data]") )
{
    QVBoxLayout * layout = new QVBoxLayout(this);

    QHBoxLayout * highLowLayout = new QHBoxLayout();

    m_lowThresh = new QDoubleSpinBox(this);
    m_lowThresh->setToolTip( tr("Set the lower threshold") );
    m_highThresh = new QDoubleSpinBox(this);
    m_highThresh->setToolTip( tr("Set the upper threshold") );
    highLowLayout->addWidget( m_lowThresh );
    highLowLayout->addWidget( m_highThresh );

    layout->addLayout( highLowLayout );
    m_seedPointTable = new QTableWidget(this);
    layout->addWidget(m_seedPointTable);

    m_seedPointTable->setColumnCount(2);
    m_seedPointTable->setHorizontalHeaderLabels( QStringList() << tr("Index") << tr("Location") );
    m_addSeedPointButton = new QPushButton( tr("Add") , this);
    m_removeSeedPointButton = new QPushButton( tr("Remove") , this);

    QHBoxLayout * addRemoveButtonLayout = new QHBoxLayout();
    addRemoveButtonLayout->addWidget( m_addSeedPointButton );
    addRemoveButtonLayout->addWidget( m_removeSeedPointButton );
    layout->addLayout( addRemoveButtonLayout );

    m_dataText = new QTextEdit( m_noDataText );
    {
        QFont font = m_dataText->currentFont();
        QFontMetrics fm( font );
        int textHeightInPixels = fm.height();
        m_dataText->setFixedHeight( textHeightInPixels );
        m_dataText->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    }
    layout->addWidget( m_dataText );

    m_applyButton = new QPushButton( tr("Apply") , this);
    layout->addWidget( m_applyButton );

    connect (m_addSeedPointButton,     SIGNAL(pressed()),
        this, SLOT(onAddSeedPointPressed ()));
    connect (m_removeSeedPointButton,     SIGNAL(pressed()),
        this, SLOT(onRemoveSeedPointPressed ()));
    connect (m_applyButton,     SIGNAL(pressed()),
        this, SLOT(onApplyButtonPressed()));

}

AlgorithmConnectedThresholdParametersWidget::~AlgorithmConnectedThresholdParametersWidget()
{
    foreach( const SeedPoint & seed, m_seedPoints ) {
        controller()->removeAnnotation( seed.annotationData );
    }
}

void AlgorithmConnectedThresholdParametersWidget::onAddSeedPointPressed()
{
    m_viewFilter.takePointer( new SingleClickEventFilter(controller(), this ) );

    m_viewState = ViewState_PickingSeedPoint;
    controller()->addViewEventFilter( m_viewFilter );
}

void AlgorithmConnectedThresholdParametersWidget::onRemoveSeedPointPressed()
{
    if ( m_seedPoints.isEmpty() ) 
        return;

    QList<QTableWidgetItem *> selection = m_seedPointTable->selectedItems();
    typedef std::vector<int> IntVector;
    IntVector rowsToDelete;
    foreach( QTableWidgetItem * item, selection ) {
        rowsToDelete.push_back( item->row() );
    }
    IntVector::iterator newEnd = std::unique( rowsToDelete.begin(), rowsToDelete.end());
    IntVector::size_type newLength = newEnd - rowsToDelete.begin();
    rowsToDelete.resize( newLength );

    // Delete from the end so that indices are untouched.
    for( IntVector::const_reverse_iterator rit( rowsToDelete.rbegin() ); rit != rowsToDelete.rend(); ++rit ){
        m_seedPointTable->removeRow( (*rit) );
        controller()->removeAnnotation( m_seedPoints.at(*rit).annotationData );
        m_seedPoints.remove( *rit );
    }
}

void AlgorithmConnectedThresholdParametersWidget::onApplyButtonPressed()
{
    dtkSmartPointer <mseg::AlgorithmConnectedThreshold> alg;
    alg.takePointer( new mseg::AlgorithmConnectedThreshold() );

    alg->setInput(this->m_data);
    foreach( const SeedPoint & seed, m_seedPoints ) {
        alg->addSeedPoint( seed.annotationData->centerWorld() );
    }

    controller()->run( alg );

}
void AlgorithmConnectedThresholdParametersWidget::setData( dtkAbstractData *dtkdata )
{   
    m_data = dtkSmartPointer<dtkAbstractData>(dtkdata);

    QString dataText;
    if ( m_data ) {
        QString patientName;
        QString studyName;
        QString seriesName;

        if ( m_data->hasMetaData( MED_METADATA_PATIENT_NAME ) ){
            patientName = dtkdata->metaDataValues(MED_METADATA_PATIENT_NAME)[0];
        }
        if ( m_data->hasMetaData( MED_METADATA_STUDY_DESCRIPTION ) ){
            studyName = dtkdata->metaDataValues(MED_METADATA_STUDY_DESCRIPTION)[0];
        }
        if ( m_data->hasMetaData( MED_METADATA_SERIES_DESCRIPTION ) ){
            seriesName = dtkdata->metaDataValues(MED_METADATA_SERIES_DESCRIPTION)[0];
        }

        dataText = patientName + '/' + studyName + '/' +seriesName;
    } else {
        // data is NULL
        dataText = m_noDataText;
    }
    m_dataText->setText( dataText );
}

void AlgorithmConnectedThresholdParametersWidget::addSeedPoint( medAbstractView *view, const QVector3D &vec )
{
    if (m_seedPoints.size() == 0 ) {
        setData( Controller::viewData(view) );
    }
    SeedPoint newSeed;
    newSeed.annotationData.takePointer( qobject_cast<mseg::SeedPointAnnotationData *>
        (dtkAbstractDataFactory::instance()->create( SEED_POINT_ANNOTATION_DATA_NAME )) );

    if ( !newSeed.annotationData ) {
        dtkDebug() << DTK_PRETTY_FUNCTION << "Failed to create annotation data";
        return;
    }
    
    newSeed.annotationData->setParentData( this->m_data.data() );

    newSeed.annotationData->setCenterWorld(vec);
    newSeed.annotationData->setColor( Qt::red );
    m_seedPoints.append( newSeed );

    int newRow = m_seedPointTable->rowCount();
    m_seedPointTable->insertRow( newRow );
    m_seedPointTable->setItem( newRow, 0, new QTableWidgetItem( QString("%1").arg(newRow) ));

    QString vecString = QString("%1,%2,%3").arg( vec.x() , 5 ).arg( vec.y() , 5 ).arg( vec.z() , 5 );
    m_seedPointTable->setItem( newRow, 1, new QTableWidgetItem( vecString )) ;

    controller()->addAnnotation(newSeed.annotationData);

}

void AlgorithmConnectedThresholdParametersWidget::onViewMousePress( medAbstractView *view, const QVector3D &vec )
{
    if ( ViewState_PickingSeedPoint == m_viewState ) {

        this->addSeedPoint( view, vec );
        controller()->removeViewEventFilter( m_viewFilter );
        m_viewState = ViewState_None;
    }
}

//static 
AlgorithmParametersWidget * 
    AlgorithmConnectedThresholdParametersWidget::createAlgorithmParametersWidget( 
        Controller *controller, QWidget *parent )
{
    return new AlgorithmConnectedThresholdParametersWidget( controller, parent );
}

QString AlgorithmConnectedThresholdParametersWidget::s_description()
{
    static const QString desc = "mseg::AlgorithmConnectedThresholdParametersWidget";
    return desc;
}

QString AlgorithmConnectedThresholdParametersWidget::s_localizedName(const QObject * trObj)
{
    if (!trObj) 
        trObj = qApp;

    return trObj->tr( "Connected Threshold" );
}







} // namespace mseg

