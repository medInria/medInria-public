#include "msegAlgorithmConnectedThresholdParametersWidget.h"
#include "msegController.h"
#include "msegView.h"
#include "msegAlgorithmConnectedThreshold.h"

#include <medAbstractViewScene.h>

#include <medCore/medAbstractView.h>

#include <dtkCore/dtkSmartPointer.h>

#include <QPushButton>
#include <QTableWidget>

#include <algorithm>
#include <set>

namespace mseg {

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

            QVector3D posImage = vscene->sceneToImagePos( mouseEvent->pos() );
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
  AlgorithmParametersWidget( controller, parent)
  {
    QVBoxLayout * layout = new QVBoxLayout(this);

    QHBoxLayout * highLowLayout = new QHBoxLayout();

    m_lowThresh = new QDoubleSpinBox(this);
    m_highThresh = new QDoubleSpinBox(this);
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

    m_dataText = new QTextEdit( tr("No Data") );
    layout->addWidget( m_dataText );

    m_applyButton = new QPushButton( tr("Apply") , this);
    layout->addWidget( m_applyButton );

    connect (m_addSeedPointButton,     SIGNAL(pressed()),
        this, SLOT(onAddSeedPointPressed ()));
    connect (m_removeSeedPointButton,     SIGNAL(pressed()),
        this, SLOT(onRemoveSeedPointPressed ()));

}

AlgorithmConnectedThresholdParametersWidget::~AlgorithmConnectedThresholdParametersWidget()
{

}

void AlgorithmConnectedThresholdParametersWidget::onAddSeedPointPressed()
{
    m_viewFilter.takePointer( new SingleClickEventFilter(controller(), this ) );

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
        m_seedPoints.remove( *rit );
    }
}

void AlgorithmConnectedThresholdParametersWidget::onApplyButtonPressed()
{
    dtkSmartPointer <mseg::AlgorithmConnectedThreshold> alg( new mseg::AlgorithmConnectedThreshold() );

    alg->setInput(m_inputData);
    foreach( const SeedPoint & seed, m_seedPoints ) {
        alg->addSeedPoint( seed.vec );
    }

    controller()->run( alg );

}

void AlgorithmConnectedThresholdParametersWidget::onViewMousePress( medAbstractView *view, const QVector3D &vec )
{

}




} // namespace mseg

