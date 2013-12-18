#include <medVtkView.h>
#include <dtkCore/dtkAbstractViewFactory.h>
#include <dtkCore/dtkAbstractViewInteractor.h>

medVtkView::medVtkView()
{

}

medVtkView::~medVtkView()
{
    
}

QString medVtkView::s_identifier()
{
    return "medVtkView";
}

QString medVtkView::identifier() const
{
    return medVtkView::s_identifier();
}

bool medVtkView::registered()
{
    return dtkAbstractViewFactory::instance()->registerViewType ( medVtkView::s_identifier(), createMedVtkView );
}

dtkAbstractView *createMedVtkView()
{
    return new medVtkView;
}

void medVtkView::addLayer (medAbstractData *data)
{        
    v3dView::addLayer(data);
}
