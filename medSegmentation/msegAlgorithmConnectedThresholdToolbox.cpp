#include "msegAlgorithmConnectedThresholdToolbox.h"

#include "msegAlgorithmConnectedThreshold.h"
#include "msegSeedPointAnnotationData.h"

#include <medAbstractView.h>
#include <medAbstractData.h>
#include <medDataIndex.h>
#include <medToolBoxSegmentation.h>
#include <medMetaDataKeys.h>
#include <medAbstractViewCoordinates.h>

#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkCore/dtkLog.h>
#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkGlobal.h>

#include <QPushButton>
#include <QTableWidget>

#include <algorithm>
#include <set>

namespace mseg {

    static const QString SEED_POINT_ANNOTATION_DATA_NAME = SeedPointAnnotationData::s_description();

class SingleClickEventFilter : public medViewEventFilter 
{
public:
    SingleClickEventFilter(medToolBoxSegmentation * controller, AlgorithmConnectedThresholdToolbox *cb ) : 
        medViewEventFilter(), 
        m_cb(cb) 
        {}

    virtual bool mousePressEvent( medAbstractView *view, QMouseEvent *mouseEvent ) MED_OVERRIDE
    {

        Q_ASSERT( view );

        medAbstractViewCoordinates * coords = view->coordinates();

        mouseEvent->accept();

        if (coords->is2D()) {
            // Convert mouse click to a 3D point in the image.

            QVector3D posImage = coords->displayToWorld( mouseEvent->posF() );
            //Project vector onto plane
            dtkAbstractData * viewData = medToolBoxSegmentation::viewData( view );
            m_cb->onViewMousePress( view, posImage );

            this->removeFromAllViews();
        }
        return mouseEvent->isAccepted();
    }
private :
    AlgorithmConnectedThresholdToolbox *m_cb;

};

AlgorithmConnectedThresholdToolbox::AlgorithmConnectedThresholdToolbox(QWidget *parent ) :
    medToolBoxSegmentationCustom( parent),
    m_viewState(ViewState_None),
    m_noDataText( tr("[No input data]") )
{
    QWidget *displayWidget = new QWidget(this);
    this->addWidget(displayWidget);

    this->setTitle(this->s_name(this));

    QVBoxLayout * layout = new QVBoxLayout(displayWidget);

    QHBoxLayout * highLowLayout = new QHBoxLayout();

    m_lowThresh = new QDoubleSpinBox(this);
    m_lowThresh->setToolTip( tr("Set the lower threshold") );
    m_highThresh = new QDoubleSpinBox(this);
    m_highThresh->setToolTip( tr("Set the upper threshold") );
    m_lowThresh->setMaximum( 20000. );
    m_highThresh->setMaximum( 20000. );

    highLowLayout->addWidget( m_lowThresh );
    highLowLayout->addWidget( m_highThresh );

    layout->addLayout( highLowLayout );
    m_seedPointTable = new QTableWidget(displayWidget);
    layout->addWidget(m_seedPointTable);

    m_seedPointTable->setColumnCount(2);
    m_seedPointTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_seedPointTable->setHorizontalHeaderLabels( QStringList() << tr("Index") << tr("Location") );
    m_addSeedPointButton = new QPushButton( tr("Add") , displayWidget);
    m_removeSeedPointButton = new QPushButton( tr("Remove") , displayWidget);

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

    m_applyButton = new QPushButton( tr("Apply") , displayWidget);
    layout->addWidget( m_applyButton );

    connect (m_addSeedPointButton,     SIGNAL(pressed()),
        this, SLOT(onAddSeedPointPressed ()));
    connect (m_removeSeedPointButton,     SIGNAL(pressed()),
        this, SLOT(onRemoveSeedPointPressed ()));
    connect (m_applyButton,     SIGNAL(pressed()),
        this, SLOT(onApplyButtonPressed()));
    connect(m_seedPointTable, SIGNAL(itemSelectionChanged()), 
        this, SLOT(onSeedPointTableSelectionChanged()));
}

AlgorithmConnectedThresholdToolbox::~AlgorithmConnectedThresholdToolbox()
{
    foreach( const SeedPoint & seed, m_seedPoints ) {
        seed.annotationData->parentData()->removeAttachedData(seed.annotationData);
    }
}

void AlgorithmConnectedThresholdToolbox::onAddSeedPointPressed()
{
    m_viewFilter = ( new SingleClickEventFilter(this->segmentationToolBox(), this ) );

    m_viewState = ViewState_PickingSeedPoint;
    this->segmentationToolBox()->addViewEventFilter( m_viewFilter );
}

void AlgorithmConnectedThresholdToolbox::onRemoveSeedPointPressed()
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
        medAnnotationData * annData = m_seedPoints.at(*rit).annotationData;
        annData->parentData()->removeAttachedData(annData);
        m_seedPoints.remove( *rit );
    }
}

void AlgorithmConnectedThresholdToolbox::onApplyButtonPressed()
{
    dtkAbstractProcessFactory *factory = dtkAbstractProcessFactory::instance();

    dtkSmartPointer <mseg::AlgorithmConnectedThreshold> alg =
            factory->createSmartPointer( AlgorithmConnectedThreshold::s_typeName() );

    alg->setHighThreshold( this->m_highThresh->value() );
    alg->setLowThreshold( this->m_lowThresh->value() );

    alg->setInput(this->m_data);
    foreach( const SeedPoint & seed, m_seedPoints ) {
        alg->addSeedPoint( seed.annotationData->centerWorld() );
    }

    this->segmentationToolBox()->run( alg );

}
void AlgorithmConnectedThresholdToolbox::setData( dtkAbstractData *dtkdata )
{   
    m_data = dtkSmartPointer<dtkAbstractData>(dtkdata);

    QString dataText;
    if ( m_data ) {
        QString patientName;
        QString studyName;
        QString seriesName;

        if ( m_data->hasMetaData( medMetaDataKeys::PatientName.key() ) ){
            patientName = dtkdata->metaDataValues(medMetaDataKeys::PatientName.key())[0];
        }
        if ( m_data->hasMetaData( medMetaDataKeys::StudyDescription.key() ) ){
            studyName = dtkdata->metaDataValues(medMetaDataKeys::StudyDescription.key())[0];
        }
        if ( m_data->hasMetaData( medMetaDataKeys::SeriesDescription.key() ) ){
            seriesName = dtkdata->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0];
        }

        dataText = patientName + '/' + studyName + '/' +seriesName;
    } else {
        // data is NULL
        dataText = m_noDataText;
    }
    m_dataText->setText( dataText );
}

void AlgorithmConnectedThresholdToolbox::addSeedPoint( medAbstractView *view, const QVector3D &vec )
{
    if (m_seedPoints.size() == 0 ) {
        setData( medToolBoxSegmentation::viewData(view) );
    }
    SeedPoint newSeed;
    newSeed.annotationData =
        dtkAbstractDataFactory::instance()->createSmartPointer( SEED_POINT_ANNOTATION_DATA_NAME );

    if ( !newSeed.annotationData ) {
        dtkDebug() << DTK_PRETTY_FUNCTION << "Failed to create annotation data";
        return;
    }
    
    newSeed.annotationData->setParentData( this->m_data.data() );

    newSeed.annotationData->setCenterWorld(vec);
    newSeed.annotationData->setSelectedColor( Qt::red );
    newSeed.annotationData->setColor( Qt::cyan );
    m_seedPoints.append( newSeed );

    int newRow = m_seedPointTable->rowCount();
    m_seedPointTable->insertRow( newRow );
    m_seedPointTable->setItem( newRow, 0, new QTableWidgetItem( QString("%1").arg(newRow) ));

    QString vecString = QString("%1,%2,%3").arg( vec.x() , 5 ).arg( vec.y() , 5 ).arg( vec.z() , 5 );
    m_seedPointTable->setItem( newRow, 1, new QTableWidgetItem( vecString )) ;

    this->m_data->addAttachedData(newSeed.annotationData);
}

void AlgorithmConnectedThresholdToolbox::onViewMousePress( medAbstractView *view, const QVector3D &vec )
{
    if ( ViewState_PickingSeedPoint == m_viewState ) {

        this->addSeedPoint( view, vec );
        this->segmentationToolBox()->removeViewEventFilter( m_viewFilter );
        m_viewState = ViewState_None;
    }
}

//static 
medToolBoxSegmentationCustom * 
    AlgorithmConnectedThresholdToolbox::createInstance(QWidget *parent )
{
    return new AlgorithmConnectedThresholdToolbox( parent );
}

QString AlgorithmConnectedThresholdToolbox::s_description()
{
    static const QString desc = "mseg::AlgorithmConnectedThresholdToolbox";
    return desc;
}

QString AlgorithmConnectedThresholdToolbox::s_identifier()
{
    static const QString id = "mseg::AlgorithmConnectedThresholdToolbox";
    return id;
}

QString AlgorithmConnectedThresholdToolbox::s_name(const QObject * trObj)
{
    if (!trObj) 
        trObj = qApp;

    return trObj->tr( "Connected Threshold" );
}

void AlgorithmConnectedThresholdToolbox::onSeedPointTableSelectionChanged()
{
    QList<QTableWidgetItem *> selection = m_seedPointTable->selectedItems();
    typedef QSet<int> IntVector;
    IntVector selectedRows;
    foreach( QTableWidgetItem * item, selection ) {
        selectedRows.insert( item->row() );
    }

    for ( int i(0); i<m_seedPoints.size(); ++i) {

        if ( selectedRows.contains(i) ) {
            m_seedPoints[i].annotationData->setSelected(true);
        } else {
            m_seedPoints[i].annotationData->setSelected(false);
        }
    }
}










} // namespace mseg

