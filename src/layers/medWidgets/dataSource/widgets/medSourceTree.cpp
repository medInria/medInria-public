#include "medSourceTree.h"

#include <medDataModel.h>
#include <medDataModelElement.h> //TODO must be renamed by medSourceItemModel
#include <medSourceItemModelPresenter.h>

#include <QWidget>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>


medSourceTree::medSourceTree(QWidget *parent) : QTreeView(parent)
{

}

medSourceTree::~medSourceTree()
{
}
