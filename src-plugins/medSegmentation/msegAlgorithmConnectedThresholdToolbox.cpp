/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <msegAlgorithmConnectedThresholdToolbox.h>

#include <msegAlgorithmConnectedThreshold.h>
#include <medSeedPointAnnotationData.h>

#include <medAbstractImageView.h>
#include <medAbstractData.h>
#include <medDataIndex.h>
#include <medSegmentationSelectorToolBox.h>
#include <medMetaDataKeys.h>
#include <medPluginManager.h>

#include <medAbstractDataFactory.h>
#include <dtkCore/dtkAbstractProcessFactory.h>
#include <dtkLog/dtkLog.h>
#include <dtkCore/dtkSmartPointer.h>
#include <dtkCore/dtkGlobal.h>

#include <QPushButton>
#include <QTableWidget>

#include <algorithm>
#include <set>


static const QString SEED_POINT_ANNOTATION_DATA_NAME = medSeedPointAnnotationData::s_description();

class SingleClickEventFilter : public medViewEventFilter
{
public:
    SingleClickEventFilter(medSegmentationSelectorToolBox * controller, AlgorithmConnectedThresholdToolbox *cb ) :
        medViewEventFilter(),
        m_cb(cb)
        {}

    virtual bool mousePressEvent( medAbstractImageView *view, QMouseEvent *mouseEvent ) MED_OVERRIDE
    {

        Q_ASSERT( view );
        mouseEvent->accept();

        if (view->is2D()) {
            // Convert mouse click to a 3D point in the image.

            QVector3D posImage = view->mapDisplayToWorldCoordinates( mouseEvent->posF() );
            //Project vector onto plane
//            medAbstractData * viewData = medSegmentationSelectorToolBox::viewData( view );
            m_cb->onViewMousePress( view, posImage );

            this->removeFromAllViews();
        }
        return mouseEvent->isAccepted();
    }
private :
    AlgorithmConnectedThresholdToolbox *m_cb;

};

AlgorithmConnectedThresholdToolbox::AlgorithmConnectedThresholdToolbox(QWidget *parent ) :
    medSegmentationAbstractToolBox( parent),
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
    if ( m_seedPoints ) {
        m_seedPoints->parentData()->removeAttachedData(m_seedPoints);
    }
}

void AlgorithmConnectedThresholdToolbox::onAddSeedPointPressed()
{
    m_viewFilter = ( new SingleClickEventFilter(this->segmentationToolBox(), this ) );

    m_viewState = ViewState_PickingSeedPoint;
    emit installEventFilterRequest(m_viewFilter);
}

void AlgorithmConnectedThresholdToolbox::onRemoveSeedPointPressed()
{
    if ( m_seedPoints->getNumberOfSeeds() == 0 )
        return;

    QList<QTableWidgetItem *> selection = m_seedPointTable->selectedItems();
    if ( selection.size() == 0 )
        return;

    typedef std::vector<int> IntVector;
    IntVector rowsToDelete;
    foreach( QTableWidgetItem * item, selection ) {
        rowsToDelete.push_back( item->row() );
    }
    std::sort( rowsToDelete.begin(), rowsToDelete.end() );

    IntVector::iterator newEnd = std::unique( rowsToDelete.begin(), rowsToDelete.end());
    IntVector::size_type newLength = newEnd - rowsToDelete.begin();
    rowsToDelete.resize( newLength );

    // Delete from the end so that indices are untouched.
    for( IntVector::const_reverse_iterator rit( rowsToDelete.rbegin() ); rit != rowsToDelete.rend(); ++rit ){
        m_seedPointTable->removeRow( (*rit) );
        m_seedPoints->removeSeed(*rit);
    }

    //for ( int i(*rowsToDelete.begin()); i<m_seedPoints->getNumberOfSeeds(); ++i) {
    //    this->updateTableRow(i);
    //}
}

void AlgorithmConnectedThresholdToolbox::onApplyButtonPressed()
{
    dtkAbstractProcessFactory *factory = dtkAbstractProcessFactory::instance();

    dtkSmartPointer <AlgorithmConnectedThreshold> alg =
            factory->createSmartPointer( AlgorithmConnectedThreshold::s_identifier() );

    alg->setHighThreshold( this->m_highThresh->value() );
    alg->setLowThreshold( this->m_lowThresh->value() );

    alg->setInput(this->m_data);
    for ( int i(0); i < m_seedPoints->getNumberOfSeeds(); ++i ) {
        alg->addSeedPoint( m_seedPoints->centerWorld(i) );
    }

//    this->segmentationToolBox()->run( alg );

}
void AlgorithmConnectedThresholdToolbox::setData( medAbstractData *medData )
{
    // disconnect existing
    if ( m_data ) {
        foreach( medAttachedData * mdata, m_data->attachedData() ) {
            if ( qobject_cast< medSeedPointAnnotationData * >(mdata) ) {
                disconnect(mdata, SIGNAL(dataModified(medAbstractData*)), this, SLOT(medAbstractData(medAnnotationData*)));
            }
        }
    }

    m_data = dtkSmartPointer<medAbstractData>(medData);

    QString dataText;
    if ( m_data ) {
        QString patientName;
        QString studyName;
        QString seriesName;

        if ( m_data->hasMetaData( medMetaDataKeys::PatientName.key() ) ){
            patientName = medData->metaDataValues(medMetaDataKeys::PatientName.key())[0];
        }
        if ( m_data->hasMetaData( medMetaDataKeys::StudyDescription.key() ) ){
            studyName = medData->metaDataValues(medMetaDataKeys::StudyDescription.key())[0];
        }
        if ( m_data->hasMetaData( medMetaDataKeys::SeriesDescription.key() ) ){
            seriesName = medData->metaDataValues(medMetaDataKeys::SeriesDescription.key())[0];
        }

        dataText = patientName + '/' + studyName + '/' +seriesName;
    } else {
        // data is NULL
        dataText = m_noDataText;
    }
    m_dataText->setText( dataText );

    m_seedPoints =
        medAbstractDataFactory::instance()->createSmartPointer( SEED_POINT_ANNOTATION_DATA_NAME );

    if ( !m_seedPoints ) {
        dtkDebug() << DTK_PRETTY_FUNCTION << "Failed to create annotation data";
        return;
    }

    m_seedPoints->setParentData( this->m_data.data() );
    m_seedPoints->setSelectedColor( Qt::red );
    m_seedPoints->setColor( Qt::cyan );

    this->m_data->addAttachedData(m_seedPoints);
    connect(m_seedPoints, SIGNAL(dataModified(medAbstractData*)), this, SLOT(onDataModified(medAbstractData*)));

}

void AlgorithmConnectedThresholdToolbox::addSeedPoint( medAbstractImageView *view, const QVector3D &vec )
{
    if (!m_seedPoints) {
//        setData( medSegmentationSelectorToolBox::viewData(view) );
    }

    int iSeed = m_seedPoints->getNumberOfSeeds();
    m_seedPoints->setCenterWorld(iSeed, vec);

    updateTableRow(iSeed);

    Q_ASSERT( m_seedPointTable->rowCount() ==  m_seedPoints->getNumberOfSeeds() ) ;

}

void AlgorithmConnectedThresholdToolbox::onViewMousePress( medAbstractImageView *view, const QVector3D &vec )
{
    if ( ViewState_PickingSeedPoint == m_viewState ) {

        this->addSeedPoint( view, vec );
//        this->segmentationToolBox()->removeViewEventFilter( m_viewFilter );
        m_viewState = ViewState_None;
    }
}

//static
medSegmentationAbstractToolBox *
    AlgorithmConnectedThresholdToolbox::createInstance(QWidget *parent )
{
    return new AlgorithmConnectedThresholdToolbox( parent );
}

QString AlgorithmConnectedThresholdToolbox::s_description()
{
    static const QString desc = tr("Connected Threshold Tool");
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

    if( selectedRows.empty() )
        m_seedPoints->setSelectedSeed(-1);
    else
        m_seedPoints->setSelectedSeed( *(selectedRows.begin()) );
}

void AlgorithmConnectedThresholdToolbox::onDataModified(medAbstractData* attached )
{
//    medSeedPointAnnotationData * spad = qobject_cast<medSeedPointAnnotationData*>(attached);
    const int numSeeds = (m_seedPoints ? m_seedPoints->getNumberOfSeeds() : 0 );
    if ( m_seedPointTable->rowCount() != numSeeds )
        m_seedPointTable->clear();
    for ( int i(0); i< numSeeds; ++i) {
        this->updateTableRow(i);
    }
}

void AlgorithmConnectedThresholdToolbox::updateTableRow( int row )
{
    QVector3D vec = m_seedPoints->centerWorld(row);

    // Ensure row is present
    if ( row >= m_seedPointTable->rowCount() ) {
        while ( row >= m_seedPointTable->rowCount() ) {
            m_seedPointTable->insertRow( m_seedPointTable->rowCount() );
        }
    }

    m_seedPointTable->setItem( row, 0, new QTableWidgetItem( QString("%1").arg(row) ));

    QString vecString = QString("%1,%2,%3").arg( vec.x() , 5 ).arg( vec.y() , 5 ).arg( vec.z() , 5 );
    m_seedPointTable->setItem( row, 1, new QTableWidgetItem( vecString )) ;
}


dtkPlugin* AlgorithmConnectedThresholdToolbox::plugin()
{
    medPluginManager* pm = medPluginManager::instance();
    dtkPlugin* plugin = pm->plugin("segmentationPlugin");
    return plugin;
}
