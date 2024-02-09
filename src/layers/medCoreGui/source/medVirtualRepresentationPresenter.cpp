#include "medVirtualRepresentationPresenter.h"

#include <medVirtualRepresentation.h>
#include <medVirtualRepresentationWidget.h>



class medVirtualRepresentationPresenterPrivate
{
public:
    medVirtualRepresentation *virtualRepresentation;
};

// QModelIndex menuGetIndexHelper(QTreeView *pTreeView, QMenu *pMenu)
// {
//     QModelIndex indexRes;

//     auto pos = pTreeView->viewport()->mapFromGlobal(pMenu->pos());
//     indexRes = pTreeView->indexAt(pos);

//     return indexRes;
// }	
	
medVirtualRepresentationPresenter::medVirtualRepresentationPresenter(medVirtualRepresentation *parent) : d(new medVirtualRepresentationPresenterPrivate())
{
    d->virtualRepresentation = parent;
}

medVirtualRepresentationPresenter::~medVirtualRepresentationPresenter()
{
}

QWidget * medVirtualRepresentationPresenter::buildWidget()
{
    return buildTree();
}

medVirtualRepresentationWidget * medVirtualRepresentationPresenter::buildTree()
{
    auto widgetRes = new medVirtualRepresentationWidget();
    
    widgetRes->create(d->virtualRepresentation);

    connect(d->virtualRepresentation, &medVirtualRepresentation::visibled, widgetRes, &medVirtualRepresentationWidget::setVisible);
    connect(d->virtualRepresentation, &medVirtualRepresentation::expandData, widgetRes->getVirtualTree(), &QTreeView::setExpanded);
    connect(d->virtualRepresentation, SIGNAL(editIndex(QModelIndex const &)), widgetRes->getVirtualTree(), SLOT(edit(const QModelIndex &)));

    return widgetRes;

}

