/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractLayeredView.h>
#include <medAbstractRoi.h>
#include <medIntParameterL.h>
#include <medRoiManagementToolBox.h>
#include <medTabbedViewContainers.h>
#include <medToolBoxFactory.h>
#include <medToolBoxTab.h>
#include <medVtkViewBackend.h>

#include <vtkImageView2D.h>

class medRoiManagementToolBoxPrivate
{
public:
    medRoiManagementToolBoxPrivate(){}

    typedef QList<medAbstractRoi*> *ListRois;
    typedef QList<medSeriesOfRoi*> *ListOfSeriesOfRois;
    typedef QPair<unsigned int, unsigned int> PairInd;

    medAbstractView *currentView;
    medToolBoxTab *layoutToolBoxTab;
    
    QTreeWidget *ListAllRois;
    QTreeWidget *ListCurrentSliceRois;
    QTreeWidget *ListPolygonRois;
    QTreeWidget *ListBrushRois;
    QTreeWidget *ListPointRois;

    QList<QTreeWidget*> listOfPages;
    
    QList<PairInd> roisSelected;
    medAbstractRoi *roi;
    
    int currentPageIndex;

    medIntParameterL *slicingParameter;
};

medRoiManagementToolBox::medRoiManagementToolBox(QWidget *parent) :
    medToolBox(parent), d(new medRoiManagementToolBoxPrivate)
{
    QWidget *allRoisTab = new QWidget();
    QVBoxLayout *allLayout = new QVBoxLayout(allRoisTab);
    d->ListAllRois = new QTreeWidget(this);
    d->ListAllRois->setSelectionMode(QAbstractItemView::ExtendedSelection);
    allLayout->addWidget(d->ListAllRois);

    QWidget *currentSliceTab = new QWidget();
    QVBoxLayout *currentSliceLayout = new QVBoxLayout(currentSliceTab);
    d->ListCurrentSliceRois = new QTreeWidget(this);
    d->ListCurrentSliceRois->setSelectionMode(QAbstractItemView::ExtendedSelection);
    currentSliceLayout->addWidget(d->ListCurrentSliceRois);
    
    QWidget *PolygonRoisTab = new QWidget();
    QVBoxLayout *polygonLayout = new QVBoxLayout(PolygonRoisTab);
    d->ListPolygonRois = new QTreeWidget(this);
    d->ListPolygonRois->setSelectionMode(QAbstractItemView::ExtendedSelection);
    polygonLayout->addWidget(d->ListPolygonRois);

    QWidget *brushRoisTab = new QWidget();
    QVBoxLayout *brushLayout = new QVBoxLayout(brushRoisTab);
    d->ListBrushRois = new QTreeWidget(this);
    d->ListBrushRois->setSelectionMode(QAbstractItemView::ExtendedSelection);
    brushLayout->addWidget(d->ListBrushRois);

    QWidget *pointRoisTab = new QWidget();
    QVBoxLayout *pointLayout = new QVBoxLayout(pointRoisTab);
    d->ListPointRois = new QTreeWidget(this);
    d->ListPointRois->setSelectionMode(QAbstractItemView::ExtendedSelection);
    pointLayout->addWidget(d->ListPointRois);

    d->layoutToolBoxTab = new medToolBoxTab();
    d->layoutToolBoxTab->addTab(allRoisTab, tr("All"));
    d->layoutToolBoxTab->addTab(currentSliceTab, tr("Current Slice"));
    d->layoutToolBoxTab->addTab(PolygonRoisTab, tr("Polygon Roi"));
    this->setTabWidget(d->layoutToolBoxTab);
    connect(d->layoutToolBoxTab,SIGNAL(currentChanged(int)),this,SLOT(saveCurrentPageIndex(int)));

    d->listOfPages.append(d->ListAllRois);
    d->listOfPages.append(d->ListCurrentSliceRois);
    d->listOfPages.append(d->ListPolygonRois);
    d->currentPageIndex = 0;
    saveCurrentPageIndex(0);

    sliceObserverTag = 0;
    orientationObserverTag = 0;
    d->currentView = nullptr;
    d->slicingParameter = nullptr;

    connect(medRoiManager::instance(),SIGNAL(roiAdded()),this,SLOT(updateDisplay()));
    connect(medRoiManager::instance(),SIGNAL(roiDeleted()),this,SLOT(updateDisplay()));
    connect(medRoiManager::instance(),SIGNAL(allRoisDeleted()),this,SLOT(updateDisplay()));
}

medRoiManagementToolBox::~medRoiManagementToolBox(void)
{
    // Stop the connection between slices's viewer and "Current Slice" tab
    if (d->currentView != nullptr)
    {
        static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D->RemoveObserver(sliceObserverTag);
        static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D->RemoveObserver(orientationObserverTag);
    }

    delete d;

    d = nullptr;
}

bool medRoiManagementToolBox::registered()
{
    return medToolBoxFactory::instance()->registerToolBox<medRoiManagementToolBox>();
}

void medRoiManagementToolBox::updateView()
{
    medAbstractView *view = this->getWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();

    if (d->currentView != nullptr) // If previous data
    {
        // Either the new view is null or not, we need to update the obervers
        static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D->RemoveObserver(sliceObserverTag);
        static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D->RemoveObserver(orientationObserverTag);
    }

    d->currentView = view;

    // For each modification of data in view ( change/remove layer, etc)
    // This is the medRoiManagementToolBox::updateView() function, not the polygonROI one:
    // the view can be seen as identical, but the layers have changed.
    if (view)
    {
        // Update "Current Slice" tab when slice changes
        sliceObserverTag = static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D->AddObserver(vtkImageView2D::SliceChangedEvent, this, &medRoiManagementToolBox::sliceChangedCallback);
        orientationObserverTag = static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D->AddObserver(vtkImageView2D::OrientationChangedEvent, this, &medRoiManagementToolBox::updateDisplay);

        d->slicingParameter = nullptr;
        foreach (medAbstractInteractor *interactor, qobject_cast<medAbstractLayeredView*>(d->currentView)->layerInteractors(0))
        {
            if ((interactor->identifier() == "medVtkViewItkDataImageInteractor") ||
                    (interactor->identifier() == "medVtkViewItkDataImage4DInteractor"))
            {
                foreach (medAbstractParameterL *parameter, interactor->linkableParameters())
                {
                    if (parameter->name() == "Slicing")
                    {
                        d->slicingParameter = qobject_cast<medIntParameterL*>(parameter);
                        break;
                    }
                }
                break;
            }
        }

        // No need to call these functions if the view is empty
        unselectRois();
        updateDisplay();
    }
}

void medRoiManagementToolBox::clear()
{
    medToolBox::clear();

    d->currentView = nullptr;
    updateDisplay();
}

void medRoiManagementToolBox::sliceChangedCallback()
{
    //if "Current Slice" tab
    if (d->currentPageIndex == 1)
    {
        d->ListCurrentSliceRois->clear();
        if (!d->currentView)
        {
            return;
        }
        vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D;

        unsigned int  currentSlice       = view2d->GetSlice();
        unsigned char currentOrientation = view2d->GetViewOrientation();

        medRoiManager *manager = medRoiManager::instance();

        if (manager->getSeriesOfRoi()->contains(d->currentView))
        {
            ListOfSeriesOfRois listOfSeries = manager->getSeriesOfRoi()->value(d->currentView);

            for(unsigned int k=0; k<(unsigned int)listOfSeries->size(); k++)
            {
                ListRois list = manager->getSeriesOfRoi()->value(d->currentView)->at(k)->getListOfRois();

                for(unsigned int i=0; i<(unsigned int)list->size(); i++)
                {
                    // Display currents Rois in the "Current Slice" tab
                    if (list->at(i)->getIdSlice() == currentSlice && list->at(i)->getOrientation() == currentOrientation)
                    {
                        QTreeWidgetItem *item = new QTreeWidgetItem(d->ListCurrentSliceRois);
                        medRoiItemWidget *widget = new medRoiItemWidget(QString::number(i+1) +  ". "  + list->at(i)->type() + " - " + list->at(i)->info(),PairInd(k,i));
                        connect(widget, SIGNAL(deleteWidget(PairInd)), this, SLOT(deleteRoi(PairInd)));
                        item->setSizeHint(0, widget->sizeHint());
                        d->ListCurrentSliceRois->insertTopLevelItem(i, item);
                        d->ListCurrentSliceRois->setItemWidget(item, 0, widget);
                    }
                }
            }
        }
    }
}

void medRoiManagementToolBox::updateDisplay()
{
    clearDisplay();

    if (d->currentView)
    {
        if (d->slicingParameter) //if interactor is medVtkViewItkDataImageInteractor (not the case with mesh, fibers)
        {
            d->slicingParameter->getSlider()->removeAllTicks();
            d->slicingParameter->getSlider()->update();
            vtkImageView2D *view2d = static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D;

            unsigned int currentSlice = view2d->GetSlice();
            unsigned char currentOrientation = view2d->GetViewOrientation();

            medRoiManager *manager = medRoiManager::instance();

            if (manager->getSeriesOfRoi()->contains(d->currentView))
            {
                ListOfSeriesOfRois listOfSeries = manager->getSeriesOfRoi()->value(d->currentView);
                for(unsigned int k=0; k<(unsigned int)listOfSeries->size(); k++)
                {
                    ListRois list = manager->getSeriesOfRoi()->value(d->currentView)->at(k)->getListOfRois();

                    if(list->size() > 0)
                    {
                        QTreeWidgetItem *seriesItem = new QTreeWidgetItem(d->ListAllRois);
                        medRoiItemWidget *widget = new medRoiItemWidget(manager->getSeriesOfRoi()->value(d->currentView)->at(k)->getName(),PairInd(k,UINT_MAX));
                        connect(widget, SIGNAL(deleteWidget(PairInd)), this, SLOT(deleteRoi(PairInd)));
                        seriesItem->setSizeHint(0, widget->sizeHint());
                        d->ListAllRois->insertTopLevelItem(k, seriesItem);
                        d->ListAllRois->setItemWidget(seriesItem, 0, widget);

                        for(unsigned int i=0; i<(unsigned int)list->size(); i++)
                        {
                            QTreeWidgetItem *item = new QTreeWidgetItem(seriesItem);
                            medRoiItemWidget *widget = new medRoiItemWidget(QString::number(i+1) +  ". "  + list->at(i)->type() + " - " + list->at(i)->info(),PairInd(k,i));
                            connect(widget, SIGNAL(deleteWidget(PairInd)), this, SLOT(deleteRoi(PairInd)));
                            item->setSizeHint(0, widget->sizeHint());
                            d->ListAllRois->setItemWidget(item, 0, widget);

                            if(list->at(i)->getOrientation() == currentOrientation)
                            {
                                d->slicingParameter->getSlider()->addTick(list->at(i)->getIdSlice());
                                d->slicingParameter->getSlider()->update();
                            }

                            if (list->at(i)->getIdSlice() == currentSlice && list->at(i)->getOrientation()==currentOrientation)
                            {
                                QTreeWidgetItem *item = new QTreeWidgetItem(d->ListCurrentSliceRois);
                                medRoiItemWidget *widget = new medRoiItemWidget(QString::number(i+1) +  ". "  + list->at(i)->type() + " - " + list->at(i)->info(),PairInd(k,i));
                                connect(widget, SIGNAL(deleteWidget(PairInd)), this, SLOT(deleteRoi(PairInd)));
                                item->setSizeHint(0, widget->sizeHint());
                                d->ListCurrentSliceRois->insertTopLevelItem(i, item);
                                d->ListCurrentSliceRois->setItemWidget(item, 0, widget);
                            }

                            if (list->at(i)->type()=="Polygon")
                            {
                                QTreeWidgetItem *item = new QTreeWidgetItem(d->ListPolygonRois);
                                medRoiItemWidget *widget = new medRoiItemWidget(QString::number(i+1) +  ". "  + list->at(i)->type() + " - " + list->at(i)->info(),PairInd(k,i));
                                connect(widget, SIGNAL(deleteWidget(PairInd)), this, SLOT(deleteRoi(PairInd)));
                                item->setSizeHint(0, widget->sizeHint());
                                d->ListPolygonRois->insertTopLevelItem(i, item);
                                d->ListPolygonRois->setItemWidget(item, 0, widget);
                            }

                            if (list->at(i)->type()=="SeedPoint")
                            {
                                QTreeWidgetItem *item = new QTreeWidgetItem(d->ListPointRois);
                                medRoiItemWidget *widget = new medRoiItemWidget(QString::number(i+1) +  ". "  + list->at(i)->type() + " - " + list->at(i)->info(),PairInd(k,i));
                                connect(widget, SIGNAL(deleteWidget(PairInd)), this, SLOT(deleteRoi(PairInd)));
                                item->setSizeHint(0, widget->sizeHint());
                                d->ListPointRois->insertTopLevelItem(i, item);
                                d->ListPointRois->setItemWidget(item, 0, widget);
                            }
                        }
                    }
                }
            }
        }
    }
}

void medRoiManagementToolBox::clearDisplay()
{
    d->ListAllRois->clear();
    d->ListCurrentSliceRois->clear();
    d->ListPolygonRois->clear();
    d->ListBrushRois->clear();
}

void medRoiManagementToolBox::saveCurrentPageIndex(int index)
{
    if (d->listOfPages.size() > d->currentPageIndex)
    {
        disconnect(d->listOfPages.at(d->currentPageIndex), SIGNAL(itemSelectionChanged()), this, SLOT(selectRois()));
    }
    d->currentPageIndex = index;
    connect(d->listOfPages.at(index),SIGNAL(itemSelectionChanged()),this,SLOT(selectRois()));
}

void medRoiManagementToolBox::selectRois()
{
    if (!d->currentView)
    {
        return;
    }

    medRoiManager *manager = medRoiManager::instance();

    unselectRois();
    QList<QTreeWidgetItem*> treeWidgetItem = d->listOfPages.at(d->currentPageIndex)->selectedItems();
    QTreeWidget *treeWidget = d->listOfPages.at(d->currentPageIndex);
    for(int i=0; i<treeWidgetItem.size(); i++)
    {
        medRoiItemWidget *itemWidget = dynamic_cast<medRoiItemWidget*>(treeWidget->itemWidget(treeWidgetItem.at(i),0));
        PairInd indexes = itemWidget->getIndex();

        if ((unsigned int)manager->getSeriesOfRoi()->value(d->currentView)->size()>indexes.first) // check we never know
        {
            if (indexes.second == UINT_MAX)
            {
                manager->getSeriesOfRoi()->value(d->currentView)->at(indexes.first)->select();
            }
            else if ((unsigned int)manager->getSeriesOfRoi()->value(d->currentView)->at(indexes.first)->getListOfRois()->size()>indexes.second)
            {
                manager->getSeriesOfRoi()->value(d->currentView)->at(indexes.first)->getListOfRois()->at(indexes.second)->select();
            }
        }

        d->roisSelected.append(indexes);

        // Display the selected slice in viewer
        QString txt = itemWidget->getRoiInfo()->text();
        QString numTxt = txt.mid(txt.indexOf("Slice")+5);
        bool ok;
        int sliceNum = numTxt.toInt(&ok, 10);
        if (ok) // do not change slice if the user selects series's title
        {
            static_cast<medVtkViewBackend*>(d->currentView->backend())->view2D->SetSlice(sliceNum-1);
        }
    }
    d->currentView->render();
}

// remove all selected rois of previous view
void medRoiManagementToolBox::unselectRois()
{
    if (!d->currentView)
    {
        return;
    }
    medRoiManager * manager = medRoiManager::instance();

    for(int i=0; i<d->roisSelected.size(); i++)
    {
        if ((unsigned int)manager->getSeriesOfRoi()->value(d->currentView)->size()>d->roisSelected.at(i).first) // check we never know
        {
            if (d->roisSelected.at(i).second == UINT_MAX)
            {
                manager->getSeriesOfRoi()->value(d->currentView)->at(d->roisSelected.at(i).first)->unselect();
            }
            else if ((unsigned int)manager->getSeriesOfRoi()->value(d->currentView)->at(d->roisSelected.at(i).first)->getListOfRois()->size()>d->roisSelected.at(i).second)
            {
                manager->getSeriesOfRoi()->value(d->currentView)->at(d->roisSelected.at(i).first)->getListOfRois()->at(d->roisSelected.at(i).second)->unselect();
            }
        }
    }
    d->roisSelected.clear();
}

void medRoiManagementToolBox::deleteRoi(PairInd indexes)
{
    if (!d->currentView)
    {
        return;
    }
    unselectRois();

    medRoiManager *manager = medRoiManager::instance();

    if (indexes.second == UINT_MAX) // get rid of the series
    {
        QList<medAbstractRoi*> rois;
        for (int i=0; i<manager->getSeriesOfRoi()->value(d->currentView)->at(indexes.first)->getListOfRois()->size(); i++)
        {
            rois.append(manager->getSeriesOfRoi()->value(d->currentView)->at(indexes.first)->getListOfRois()->at(i));
        }
        DeleteSeveralRoisCommand *command = new DeleteSeveralRoisCommand(d->currentView, rois, "Polygon rois", "deleteSeveralRois");
        manager->addToUndoRedoStack(command);

        delete manager->getSeriesOfRoi()->value(d->currentView)->takeAt(indexes.first);
    }
    else if (((unsigned int)manager->getSeriesOfRoi()->value(d->currentView)->size()>indexes.first)
        && ((unsigned int)manager->getSeriesOfRoi()->value(d->currentView)->at(indexes.first)->getListOfRois()->size()>indexes.second))
    {
        // delete the selected ROI
        DeleteRoiCommand *command = new DeleteRoiCommand(d->currentView,manager->getSeriesOfRoi()->value(d->currentView)->at(indexes.first)->getListOfRois()->at(indexes.second),"Polygon rois","deleteRoi");
        manager->addToUndoRedoStack(command);

        // if no more ROI, remove the serie
        if (manager->getSeriesOfRoi()->value(d->currentView)->at(indexes.first)->getListOfRois()->size() == 0)
        {
            delete manager->getSeriesOfRoi()->value(d->currentView)->takeAt(0);
        }
    }
    
    d->currentView->render();
    updateDisplay();
}

medAbstractView * medRoiManagementToolBox::getCurrentView()
{
    return d->currentView;
}

QList<medRoiManagementToolBox::PairInd> medRoiManagementToolBox::getSelectedRois()
{
    return d->roisSelected;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
medRoiItemWidget::medRoiItemWidget(QString name, PairInd indexes, QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    
    // Delete button
    QPushButton *deleteButton = new QPushButton("D",this);
    deleteButton->setToolTip("Delete");
    deleteButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    roiInfo = new QLabel(name, this);
    
    layout->addWidget(roiInfo);
    layout->addStretch();
    layout->addWidget(deleteButton);

    this->indexes = indexes;

    connect(deleteButton, SIGNAL(clicked(bool)),this,SLOT(emitDeleteWidget()));
}

medRoiItemWidget::~medRoiItemWidget()
{
}

medRoiItemWidget::PairInd medRoiItemWidget::getIndex()
{
    return indexes;
}

QLabel* medRoiItemWidget::getRoiInfo()
{
    return roiInfo;
}

void medRoiItemWidget::emitDeleteWidget()
{
    emit deleteWidget(indexes);
}
