#include "msegToolbox.h"

#include "msegController.h"

#include <medGui/medProgressionStack.h>

#include <QPushButton>

namespace mseg {

class ToolboxPrivate
{
public:
    medProgressionStack *progression_stack;
    QBoxLayout *algorithmParameterLayout;
    QComboBox * algorithmCombo;
    Controller * controller;
};


Toolbox::Toolbox(QWidget *parent) : 
    medToolBox(parent), 
    d(new ToolboxPrivate)
{
    d->controller = NULL;

    QWidget *displayWidget = new QWidget(this);
    d->algorithmParameterLayout = new QBoxLayout( QBoxLayout::LeftToRight );

    QVBoxLayout *displayLayout = new QVBoxLayout(displayWidget);

    d->algorithmCombo = new QComboBox( this );

    this->setTitle("Segmentation");

    // progression stack
    d->progression_stack = new medProgressionStack(displayWidget);
    displayLayout->addWidget( d->algorithmCombo );
    displayLayout->addLayout( d->algorithmParameterLayout );
    displayLayout->addWidget( d->progression_stack );

    this->addWidget(displayWidget);

    connect( d->algorithmCombo, SIGNAL( currentIndexChanged(int) ), this, SLOT( onAlgorithmSelected( int )) );

}

Toolbox::~Toolbox()
{
    delete d;
    d = NULL;
}

void Toolbox::update (dtkAbstractView *view)
{
}

medProgressionStack * Toolbox::progressionStack()
{
    return d->progression_stack;
}

void Toolbox::onAlgorithmAdded( const QString & algName )
{
    d->algorithmCombo->addItem( d->controller->localizedNameForAlgorithm(algName), QVariant(algName) );
}

void Toolbox::onAlgorithmSelected( int index )
{
    QString algName =  (d->algorithmCombo->itemData( index ) ).toString();
    if ( !algName.isEmpty() ) {
        emit algorithmSelected( algName );
    }
}

void Toolbox::onAlgorithmSelected( const QString & algName )
{
}

void Toolbox::setAlgorithmParameterWidget( QWidget * widget )
{
    while ( ! d->algorithmParameterLayout->isEmpty() ) {
        QScopedPointer<QLayoutItem> item (d->algorithmParameterLayout->itemAt(0));
        d->algorithmParameterLayout->removeItem( item.data() );
    }
    d->algorithmParameterLayout->addWidget( widget );
}

void Toolbox::setController( mseg::Controller * controller )
{
    d->controller = controller;
}







} // namespace mseg
