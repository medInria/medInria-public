/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medWorkspaceArea.h"
#include "medWorkspaceArea_p.h"

#include "medDatabaseDataSource.h"
#include "medDataSourceManager.h"
#include "mscExportVideoDialog.h"

#include <dtkCore/dtkAbstractProcessFactory.h>

#include <medAbstractImageView.h>
#include <medAbstractProcess.h>
#include <medParameterGroupManager.h>
#include <medSettingsManager.h>
#include <medTabbedViewContainers.h>
#include <medTimeLineParameter.h>
#include <medToolBox.h>
#include <medToolBoxContainer.h>
#include <medWorkspaceFactory.h>

medWorkspaceArea::medWorkspaceArea(QWidget *parent) : QWidget(parent), d(new medWorkspaceAreaPrivate)
{
    d->selectionToolBox = NULL;

    // -- Internal logic
    d->currentWorkspaceName = "";
    d->currentWorkspace = 0;

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

    this->addDatabaseView(medDataSourceManager::instance()->databaseDataSource());
    connect(medDataSourceManager::instance(), SIGNAL(open(medDataIndex)), this, SIGNAL(open(medDataIndex)));

    if (!d->splitter->restoreState(medSettingsManager::instance()->value("medWorkspaceArea", "splitterState").toByteArray()))
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

medWorkspaceArea::~medWorkspaceArea(void)
{
    medSettingsManager::instance()->setValue("workspaceAreaSplitter", "state", d->splitter->saveState());
    delete d;
    d = NULL;
}

QPixmap medWorkspaceArea::grabScreenshot()
{
    medAbstractView* currentView = currentWorkspace()->stackedViewContainers()->getFirstSelectedContainerView();
    if (currentView != nullptr)
    {
        return QPixmap::grabWindow(currentView->viewWidget()->winId());
    }
    return QPixmap();
}

void medWorkspaceArea::grabVideo()
{
    medAbstractView* view = this->currentWorkspace()->stackedViewContainers()->getFirstSelectedContainerView();
    if (view)
    {
        medAbstractProcess* process = qobject_cast<medAbstractProcess*>(dtkAbstractProcessFactory::instance()->create("msc::ExportVideo"));
        medAbstractImageView* iview = dynamic_cast<medAbstractImageView*>(view);
        medTimeLineParameter* timeLine = iview->timeLineParameter();

        // Get number of frames for dialog window
        int numberOfFrames = 1;
        if (timeLine)
        {
            numberOfFrames = timeLine->numberOfFrame();
        }

        QVector<int> userParameters = getExportVideoDialogParameters(numberOfFrames);

        if (userParameters.at(0) == 1) // User is ok to run the video export
        {
            // Needed to remove the shadow of the dialog window
            iview->render();

            int screenshotCount = 0;

            if (userParameters.at(1) == 0) // Time video (for 4D datasets)
            {
                timeLine->unlockTimeLine();

                for (int f=0; f<timeLine->numberOfFrame(); f+=userParameters.at(2))
                {
                    timeLine->setFrame(f);
                    runExportVideoProcess(process, screenshotCount);
                    screenshotCount++;
                }

                timeLine->lockTimeLine();
            }
            else // Rotation video
            {
                for (double rotation=0.0; rotation<360.0; rotation+=userParameters.at(2))
                {
                    iview->setRotation(rotation);
                    runExportVideoProcess(process, screenshotCount);
                    screenshotCount++;
                }
            }

            // Compute the video and export it
            process->update();

            delete process;
        }
    }
}

QVector<int> medWorkspaceArea::getExportVideoDialogParameters(int numberOfFrames)
{
    mscExportVideoDialog *exportDialog = new mscExportVideoDialog(this, numberOfFrames);

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

void medWorkspaceArea::runExportVideoProcess(medAbstractProcess* process, int screenshotCount)
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

void medWorkspaceArea::addToolBox(medToolBox *toolbox)
{
    if(!toolbox)
        return;

    d->toolBoxContainer->addToolBox(toolbox);
    toolbox->show();
}

void medWorkspaceArea::insertToolBox(int index, medToolBox *toolbox)
{
    if(!toolbox)
        return;

    d->toolBoxContainer->insertToolBox(index, toolbox);
    toolbox->show();
}

void medWorkspaceArea::removeToolBox(medToolBox *toolbox)
{
    if(!toolbox)
        return;

    toolbox->hide();
    d->toolBoxContainer->removeToolBox(toolbox);
}

void medWorkspaceArea::setCurrentWorkspace(medAbstractWorkspace *workspace)
{
    if (d->currentWorkspace == workspace)
        return;

    if (!d->workspaces.contains(workspace->identifier()))
        this->setupWorkspace(workspace->identifier());

    this->disconnect(this, SIGNAL(open(medDataIndex)), d->currentWorkspace, 0);

    d->currentWorkspace = workspace;
    connect(this, SIGNAL(open(medDataIndex)), d->currentWorkspace, SLOT(open(medDataIndex)));

    //clean toolboxes
    d->toolBoxContainer->hide();
    d->toolBoxContainer->clear();
    this->switchToStackedViewContainers(workspace->stackedViewContainers());

    //setup database visibility
    d->navigatorContainer->setVisible(workspace->isDatabaseVisible());

    // add toolboxes
    d->toolBoxContainer->addToolBox(workspace->selectionToolBox());
    workspace->selectionToolBox()->show();

    foreach (medToolBox * toolbox, workspace->toolBoxes())
    {
        d->toolBoxContainer->addToolBox(toolbox);
        toolbox->show();
    }
    d->toolBoxContainer->setVisible(workspace->areToolBoxesVisible());

    medParameterGroupManager::instance()->setCurrentWorkspace(workspace->identifier());

}

void medWorkspaceArea::setCurrentWorkspace(const QString &id)
{
    if (!d->workspaces.contains(id))
        this->setupWorkspace(id);

    this->setCurrentWorkspace(d->workspaces.value(id));
}

medAbstractWorkspace* medWorkspaceArea::currentWorkspace()
{
    return d->currentWorkspace;
}

void medWorkspaceArea::setupWorkspace(const QString &id)
{
    if (d->workspaces.contains(id))
        return;

    medAbstractWorkspace *workspace = NULL;

    workspace = medWorkspaceFactory::instance()->createWorkspace(id, this);
    if (workspace)
        d->workspaces.insert(id, workspace);
    else
    {
        qWarning()<< "Workspace " << id << " couldn't be created";
        return;
    }
    workspace->setupViewContainerStack();
    workspace->setInitialGroups();
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
        qWarning() << "No stack to switch to";
        return;
    }

    if (-1 == d->stack->indexOf(stack))
        d->stack->addWidget(stack);

    d->stack->setCurrentWidget(stack);
}
