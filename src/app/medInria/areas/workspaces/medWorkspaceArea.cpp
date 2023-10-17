/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medWorkspaceArea.h"
#include "medWorkspaceArea_p.h"

#include <medAbstractData.h>
#include <medAbstractDataFactory.h>
#include <medAbstractDbController.h>
#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medAbstractView.h>
#include <medDatabaseDataSource.h>
#include <medDatabaseNonPersistentController.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medDataSourceManager.h>
#include <medExportVideoDialog.h>
#include <medMetaDataKeys.h>
#include <medParameterGroupManagerL.h>
#include <medSettingsManager.h>
#include <medTabbedViewContainers.h>
#include <medTimeLineParameterL.h>
#include <medToolBox.h>
#include <medToolBoxContainer.h>
#include <medViewContainerSplitter.h>
#include <medViewContainer.h>
#include <medViewFactory.h>
#include <medWorkspaceFactory.h>

#include <dtkCoreSupport/dtkAbstractProcessFactory.h>

#include <QApplication>
#include <QtGui>
#include <QtWidgets>
#include <QImage>
#include <QtOpenGL/QGLWidget>

medWorkspaceArea::medWorkspaceArea(QWidget *parent) : QWidget(parent), d(new medWorkspaceAreaPrivate)
{
    d->selectionToolBox = nullptr;

    // -- Internal logic
    d->currentWorkspaceName = "";
    d->currentWorkspace = nullptr;

    d->splitter = new QSplitter(this);
    // needed to restore state.
    d->splitter->setObjectName("medWorkspaceAreaSplitter");
    d->splitter->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d->splitter->setHandleWidth(2);

    // -- User interface setup
    d->stack = new QStackedWidget(this);
    d->stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Setting up toolbox container
    d->toolBoxContainer = new medToolBoxContainer(this);
    d->toolBoxContainer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    d->toolBoxContainer->setMinimumWidth(385);

    // Setting up view container
    d->viewContainer = new QWidget(this);
    QVBoxLayout *viewContainerLayout = new QVBoxLayout(d->viewContainer);
    viewContainerLayout->setContentsMargins(0, 0, 0, 0);
    viewContainerLayout->addWidget(d->stack);

    // Setting up navigator
    d->navigatorContainer = new QWidget(this);
    d->navigatorContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    d->navigatorContainer->setMinimumWidth(186);
    d->navigatorContainer->setMaximumWidth(320);
    d->navigatorContainer->setContentsMargins(0,0,0,0);

    //Set up viewer layout
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(d->splitter);
    setLayout(layout);
    layout->setContentsMargins(1, 0, 1, 0);
    d->splitter->addWidget(d->navigatorContainer);
    d->splitter->addWidget(d->viewContainer);
    d->splitter->addWidget(d->toolBoxContainer);

    this->addDatabaseView(medDataSourceManager::instance().databaseDataSource());
    connect(&medDataSourceManager::instance(), SIGNAL(open(medDataIndex)), this, SIGNAL(open(medDataIndex)));

    if (!d->splitter->restoreState(medSettingsManager::instance().value("medWorkspaceArea", "splitterState").toByteArray()))
    {
        d->splitter->setOrientation(Qt::Horizontal);
        //viewcontainer size
        int viewContainerSize = QWIDGETSIZE_MAX -
            d->navigatorContainer->minimumWidth()-
            d->toolBoxContainer->minimumWidth();
        QList<int> sizes;
        sizes.append(d->navigatorContainer->minimumWidth());
        sizes.append(viewContainerSize);
        sizes.append(d->toolBoxContainer->minimumWidth());
        d->splitter->setSizes(sizes);
    }
}

medWorkspaceArea::~medWorkspaceArea()
{
    medSettingsManager::instance().setValue("workspaceAreaSplitter", "state", d->splitter->saveState());
    delete d;
    d = nullptr;
}

QPixmap medWorkspaceArea::grabScreenshot()
{
    medTabbedViewContainers *tabbedContainers = currentWorkspace()->tabbedViewContainers();
    QList<medViewContainer*> currentContainerList = tabbedContainers->containersInTab(tabbedContainers->currentIndex());

    QList<medAbstractView*> selectedValidViewList;
    for (medViewContainer *viewContainer : currentContainerList)
    {
        // Only takes into account selected non empty views
        if ((viewContainer->view() != nullptr) && viewContainer->isSelected())
        {
            selectedValidViewList.push_back(viewContainer->view());
        }
    }

    if (selectedValidViewList.count() != 0)
    {
        if (selectedValidViewList.count() == 1)
        {
            // Only one view
            medAbstractView *currentView = selectedValidViewList.at(0);
            QPixmap screenshot(currentView->viewWidget()->size());
            currentView->viewWidget()->render(&screenshot);

            return screenshot;
        }
        else
        {
            // Multiple selected views, we get the screenshot of the whole tab/container
            return QPixmap::grabWindow(this->currentWorkspace()->tabbedViewContainers()->currentWidget()->winId());
        }
    }
    return QPixmap();

}

void medWorkspaceArea::grabVideo()
{
    medAbstractView *view = this->currentWorkspace()->tabbedViewContainers()->getFirstSelectedContainerView();
    if (view)
    {
        medAbstractProcessLegacy *process = qobject_cast<medAbstractProcessLegacy*>(dtkAbstractProcessFactory::instance()->create("med::ExportVideo"));
        medAbstractImageView *iview = dynamic_cast<medAbstractImageView*>(view);
        medTimeLineParameterL *timeLine = iview->timeLineParameter();

        // Get number of frames for dialog window
        int numberOfFrames = 1;
        if (timeLine)
        {
            numberOfFrames = timeLine->numberOfFrame();
        }

        int numberOfSlices = 1;
        medAbstractLayeredView *layeredView = dynamic_cast<medAbstractLayeredView*>(view);
        if (layeredView)
        {
            medAbstractData *data = layeredView->layerData(layeredView->currentLayer());
            if (data)
            {
                medAbstractImageData *medData = dynamic_cast <medAbstractImageData *> (data);
                if (medData)
                {
                    numberOfSlices = medData->zDimension();
                }
            }
        }

        QVector<int> userParameters = getExportVideoDialogParameters(numberOfFrames, numberOfSlices);

        if (userParameters.at(0) == 1) // User is ok to run the video export
        {
            // Needed to remove the shadow of the dialog window
            iview->render();

            int screenshotCount = 0;

            QApplication::setOverrideCursor(Qt::WaitCursor);
            QApplication::processEvents();

            switch(userParameters.at(1))
            {
                // Time video (for 4D datasets)
                case 0:
                default:
                {
                    timeLine->unlockTimeLine();

                    for (int f=0; f<timeLine->numberOfFrame(); f+=userParameters.at(2))
                    {
                        timeLine->setFrame(f);
                        runExportVideoProcess(process, screenshotCount);
                        screenshotCount++;
                    }

                    timeLine->lockTimeLine();
                    break;
                }
            
                // Rotation video
                case 1:
                {
                    bool res = true;
                    for (double rotation=0.0; rotation<360.0; rotation+=userParameters.at(2))
                    {
                        res = iview->setRotation(rotation);
                        if (res)
                        {
                            runExportVideoProcess(process, screenshotCount);
                            screenshotCount++;
                        }
                        else
                        {
                            break;
                        }
                    }
                    break;
                }

                // Slice video
                case 2:
                {
                    for (int slice=0; slice<numberOfSlices; ++slice)
                    {
                        iview->setSlice(slice);
                        runExportVideoProcess(process, screenshotCount);
                        screenshotCount++;
                    }
                }
            }

            QApplication::restoreOverrideCursor();

            // Compute the video and export it
            process->update();

            delete process;
        }
    }
}

QVector<int> medWorkspaceArea::getExportVideoDialogParameters(int numberOfFrames, int numberOfSlices)
{
    medExportVideoDialog *exportDialog = new medExportVideoDialog(this, numberOfFrames, numberOfSlices);

    QVector<int> results;

    if(exportDialog->exec() == QDialog::Accepted)
    {
        results.append(1); // Accepted

        QVector<int> resultsFromDialog = exportDialog->value();
        results.append(resultsFromDialog.at(0));
        results.append(resultsFromDialog.at(1));
    }
    else
    {
        results.append(0); // Cancelled
    }

    delete exportDialog;

    return results;
}

void medWorkspaceArea::runExportVideoProcess(medAbstractProcessLegacy *process, int screenshotCount)
{
    // Get the current state of the view
    QPixmap currentPixmap = grabScreenshot();
    QImage currentQImage = currentPixmap.toImage();

    int arraySize = 2
            + (3
               * currentQImage.size().width()
               * currentQImage.size().height());

    std::vector<int> pixelListOfCurrentScreenshot (arraySize);

    // Two firsts values are width and height of the image
    pixelListOfCurrentScreenshot[0] = currentQImage.size().width();
    pixelListOfCurrentScreenshot[1] = currentQImage.size().height();

    for (int i=0; i<currentQImage.size().width(); ++i)
    {
        for (int j=0; j<currentQImage.size().height(); ++j)
        {
            QColor color(currentQImage.pixel(i, j));
            int index1D = 2 + (i * currentQImage.size().height() + j)*3;
            pixelListOfCurrentScreenshot[index1D    ] = color.red();
            pixelListOfCurrentScreenshot[index1D + 1] = color.green();
            pixelListOfCurrentScreenshot[index1D + 2] = color.blue();
        }
    }

    // Send for each screenshot the R, G, B int array to the process
    process->setParameter(pixelListOfCurrentScreenshot.data(), screenshotCount);
}

bool medWorkspaceArea::setCurrentWorkspace(medAbstractWorkspaceLegacy *workspace)
{
    if (d->currentWorkspace == workspace)
        return true;

    if (!d->workspaces.contains(workspace->identifier()))
    {
        if (!this->setupWorkspace(workspace->identifier()))
        {
            return false;
        }
    }

    if (d->currentWorkspace)
    {
        disconnect(this, SIGNAL(open(medDataIndex)), d->currentWorkspace, 0);
        disconnect(d->currentWorkspace, nullptr, this, nullptr);
    }
    d->currentWorkspace = workspace;
    connect(this, SIGNAL(open(medDataIndex)), d->currentWorkspace, SLOT(open(medDataIndex)));
    connect(d->currentWorkspace, &medAbstractWorkspaceLegacy::toolBoxInserted, d->toolBoxContainer, &medToolBoxContainer::insertToolBox);
    connect(d->currentWorkspace, &medAbstractWorkspaceLegacy::toolBoxRemoved, d->toolBoxContainer, &medToolBoxContainer::removeToolBox);

    //clean toolboxes
    d->toolBoxContainer->hide();
    d->toolBoxContainer->clear();
    this->switchToStackedViewContainers(workspace->tabbedViewContainers());

    //setup database visibility
    d->navigatorContainer->setVisible(workspace->isDatabaseVisible());

    // add toolboxes
    for(medToolBox * toolbox : workspace->toolBoxes())
    {
        d->toolBoxContainer->addToolBox(toolbox);
    }
    d->toolBoxContainer->setVisible(workspace->areToolBoxesVisible());

    medParameterGroupManagerL::instance().setCurrentWorkspace(workspace->identifier());

    return true;
}

bool medWorkspaceArea::setCurrentWorkspace(const QString &id)
{
    if (!d->workspaces.contains(id))
    {
        if (!this->setupWorkspace(id))
        {
            return false;
        }
    }

    return this->setCurrentWorkspace(d->workspaces.value(id));
}

medAbstractWorkspaceLegacy* medWorkspaceArea::currentWorkspace()
{
    return d->currentWorkspace;
}

bool medWorkspaceArea::setupWorkspace(const QString &id)
{
    if (d->workspaces.contains(id))
        return true;

    medAbstractWorkspaceLegacy *workspace = nullptr;

    workspace = medWorkspaceFactory::instance()->createWorkspace(id, this);
    if (workspace)
    {
        d->workspaces.insert(id, workspace);
    }
    else
    {
        qWarning()<< "Workspace " << id << " couldn't be created";
        return false;
    }
    workspace->setupTabbedViewContainer();
    workspace->setInitialGroups();

    return true;
}

void medWorkspaceArea::addDatabaseView(medDatabaseDataSource* dataSource)
{
    QVBoxLayout *databaseViewLayout = new QVBoxLayout;
    databaseViewLayout->setSpacing(0);
    databaseViewLayout->setContentsMargins(0,0,0,0);

    databaseViewLayout->addWidget(dataSource->compactViewWidget());
    d->navigatorContainer->setLayout(databaseViewLayout);

    dataSource->compactViewWidget()->resize(dataSource->compactViewWidget()->width(), dataSource->compactViewWidget()->height());
    //little tricks to force to recompute the stylesheet.
    dataSource->compactViewWidget()->setStyleSheet("/* */");

    connect(dataSource->compactViewWidget(), SIGNAL(open(const medDataIndex&)),
            this, SIGNAL(open(const medDataIndex&)),
            Qt::UniqueConnection);
}

void medWorkspaceArea::switchToStackedViewContainers(medTabbedViewContainers* stack)
{
    if(!stack )
    {
        dtkWarn() << "No stack to switch to";
        return;
    }

    if (-1 == d->stack->indexOf(stack))
        d->stack->addWidget(stack);

    d->stack->setCurrentWidget(stack);
}
