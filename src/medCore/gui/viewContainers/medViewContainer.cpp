/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medViewContainer.h>


#include <QHBoxLayout>
#include <QPushButton>
#include <QUuid>
#include <QVBoxLayout>

#include <medAbstractImageView.h>
#include <medAbstractView.h>
#include <medDataIndex.h>
#include <medDataManager.h>
#include <medLayoutChooser.h>
#include <medMetaDataKeys.h>
#include <medPoolIndicator.h>
#include <medSettingsManager.h>
#include <medTriggerParameter.h>
#include <medViewContainerManager.h>
#include <medViewContainerSplitter.h>
#include <medViewFactory.h>

class medViewContainerPrivate
{
public:
    QUuid uuid;

    QWidget *defaultWidget;
    QWidget *viewToolbar;

    medAbstractView* view;
    medViewContainerSplitter* parent;

    bool userOpenable;
    bool selected;
    bool userSplittable;
    medViewContainer::ClosingMode closingMode;
    bool multiLayer;
    bool userPoolable;
    QList<QUuid> expectedUuids;

    QGridLayout* mainLayout;
    QHBoxLayout* toolBarLayout;
    QHBoxLayout* viewToolbarContainerLayout;
    QMenu *toolBarMenu;
    QPushButton *menuButton;

    medLayoutChooser *presetLayoutChooser;
    QMenu* presetMenu;

    QAction *openAction;
    QAction* vSplitAction;
    QAction* hSplitAction;
    QAction* fourSplitAction;
    QPushButton* closeContainerButton;
    QAction* histogramAction;
    QAction* maximizedAction;
    QAction* saveSceneAction;

    QString defaultStyleSheet;
    QString highlightColor;

    medPoolIndicator* poolIndicator;

    ~medViewContainerPrivate()
    {
        if(view)
            delete view;
    }
};


medViewContainer::medViewContainer(medViewContainerSplitter *parent): QFrame(parent),
    d(new medViewContainerPrivate)
{
    d->parent = parent;

    d->uuid = QUuid::createUuid();
    medViewContainerManager::instance()->registerNewContainer(this);

    d->view = NULL;
    d->viewToolbar = NULL;

    d->defaultWidget = new QWidget;
    d->defaultWidget->setObjectName("defaultWidget");
    QLabel *defaultLabel = new QLabel(tr("Drag'n drop series/study here from the left panel or"));
    QPushButton *openButton= new QPushButton(tr("Open a file from your system"));
    QPushButton *sceneButton= new QPushButton(tr("Open a scene from your system"));
    QVBoxLayout *defaultLayout = new QVBoxLayout(d->defaultWidget);
    defaultLayout->addWidget(defaultLabel);
    defaultLayout->addWidget(openButton);
    defaultLayout->addWidget(sceneButton);
    connect(openButton, SIGNAL(clicked()), this, SLOT(openFromSystem()));
    connect(sceneButton, SIGNAL(clicked()), this, SLOT(loadScene()));

    d->menuButton = new QPushButton(this);
    d->menuButton->setIcon(QIcon(":/pixmaps/tools.png"));
    d->menuButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->menuButton->setToolTip(tr("Tools"));

    d->toolBarMenu = new QMenu(this);
    connect(d->menuButton, SIGNAL(clicked()), this, SLOT(popupMenu()));

    d->openAction = new QAction(tr("Open"), d->toolBarMenu);
    d->openAction->setIcon(QIcon(":/pixmaps/open.png"));
    d->openAction->setToolTip(tr("Open a file from your system"));
    d->openAction->setIconVisibleInMenu(true);
    connect(d->openAction, SIGNAL(triggered()), this, SLOT(openFromSystem()));

    d->closeContainerButton = new QPushButton(this);
    d->closeContainerButton->setIcon(QIcon(":/pixmaps/closebutton.png"));
    d->closeContainerButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->closeContainerButton->setToolTip(tr("Close"));
    d->closeContainerButton->setFocusPolicy(Qt::NoFocus);

    // Split actions
    d->vSplitAction = new QAction(tr("V split"), d->toolBarMenu);
    d->vSplitAction->setIcon(QIcon(":/pixmaps/splitbutton_vertical.png"));
    d->vSplitAction->setToolTip(tr("Split vertically"));
    d->vSplitAction->setIconVisibleInMenu(true);
    connect(d->vSplitAction, SIGNAL(triggered()), this, SIGNAL(vSplitRequest()));

    d->hSplitAction = new QAction(tr("H split"), d->toolBarMenu);
    d->hSplitAction->setIcon(QIcon(":/pixmaps/splitbutton_horizontal.png"));
    d->hSplitAction->setToolTip(tr("Split horizontally"));
    d->hSplitAction->setIconVisibleInMenu(true);
    connect(d->hSplitAction, SIGNAL(triggered()), this, SIGNAL(hSplitRequest()));

    d->fourSplitAction = new QAction(tr("4 split"), d->toolBarMenu);
    d->fourSplitAction->setIcon(QIcon(":/icons/fourViews.png"));
    d->fourSplitAction->setToolTip(tr("Split in 4 views"));
    d->fourSplitAction->setIconVisibleInMenu(true);
    connect(d->fourSplitAction, SIGNAL(triggered()), this, SIGNAL(requestFourSplit()));
    d->fourSplitAction->setEnabled(false);

    // Histogram actions
    d->histogramAction = new QAction(tr("Open Histogram"), d->toolBarMenu);
    d->histogramAction->setCheckable(true);
    d->histogramAction->setIcon(QIcon(":/icons/Gaussian_Filter.png"));
    d->histogramAction->setToolTip("Open a histogram");
    d->histogramAction->setIconVisibleInMenu(true);
    d->histogramAction->setEnabled(false);

    // make it a parameter to get synch between state of the container and the maximized button.
    d->maximizedAction = new QAction(tr("Maximize"), d->toolBarMenu);
    d->maximizedAction->setToolTip("Toggle maximized / unmaximized");
    d->maximizedAction->setCheckable(true);
    QIcon maximizedIcon(":/icons/maximize.svg");
    maximizedIcon.addFile(":/icons/un_maximize.svg",
                        QSize(16,16),
                        QIcon::Normal,
                        QIcon::On);

    d->maximizedAction->setIcon(maximizedIcon);
    d->maximizedAction->setIconVisibleInMenu(true);
    connect(d->maximizedAction, SIGNAL(toggled(bool)), this, SLOT(toggleMaximized(bool)));
    d->maximizedAction->setEnabled(true);

    d->saveSceneAction = new QAction(tr("Save scene"), d->toolBarMenu);
    d->saveSceneAction->setToolTip(tr("Save container content as is."));
    d->saveSceneAction->setIcon(QIcon(":icons/saveScene.png"));
    d->saveSceneAction->setIconVisibleInMenu(true);
    connect(d->saveSceneAction, SIGNAL(triggered()), this, SLOT(saveScene()));
    d->saveSceneAction->setEnabled(false);

    // Presets
    d->presetMenu = new QMenu(tr("Presets"),this);
    d->presetMenu->setToolTip(tr("Split into presets"));
    d->presetMenu->setIcon(QIcon(":/icons/splitPresets.png"));

    d->presetLayoutChooser = new medLayoutChooser(this);
    connect(d->presetLayoutChooser, SIGNAL(selected(unsigned int,unsigned int)), this, SLOT(splitContainer(unsigned int,unsigned int)));

    QVBoxLayout *presetMenuLayout = new QVBoxLayout;
    presetMenuLayout->setContentsMargins(0,0,0,0);
    presetMenuLayout->addWidget(d->presetLayoutChooser);
    d->presetMenu->setLayout(presetMenuLayout);

    // Sort toolbar
    d->toolBarMenu = new QMenu(this);
    d->toolBarMenu->addActions(QList<QAction*>() << d->openAction);
    d->toolBarMenu->addSeparator();
    d->toolBarMenu->addActions(QList<QAction*>() << d->vSplitAction << d->hSplitAction << d->fourSplitAction);
    d->toolBarMenu->addMenu(d->presetMenu);
    d->toolBarMenu->addSeparator();
    d->toolBarMenu->addActions(QList<QAction*>() << d->maximizedAction);
    d->toolBarMenu->addSeparator();
    d->toolBarMenu->addActions(QList<QAction*>() << d->saveSceneAction);
    d->toolBarMenu->addSeparator();
    d->toolBarMenu->addActions(QList<QAction*>() << d->histogramAction);

    d->poolIndicator = new medPoolIndicator;

    QWidget* toolBar = new QWidget(this);
    toolBar->setObjectName("containerToolBar");
    toolBar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    d->toolBarLayout = new QHBoxLayout(toolBar);
    d->toolBarLayout->setContentsMargins(0, 0, 0, 0);
    d->toolBarLayout->setSpacing(2);

    QWidget *viewToolbarContainer = new QWidget(this);
    viewToolbarContainer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->viewToolbarContainerLayout = new QHBoxLayout(viewToolbarContainer);
    d->viewToolbarContainerLayout->setContentsMargins(2, 0, 2, 0);
    d->viewToolbarContainerLayout->setSpacing(2);

    QWidget *containerToolbar = new QWidget(this);
    containerToolbar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    QHBoxLayout *containerToolbarLayout  = new QHBoxLayout(containerToolbar);
    containerToolbarLayout->setContentsMargins(2, 0, 2, 0);
    containerToolbarLayout->setSpacing(2);
    containerToolbarLayout->addWidget(d->poolIndicator);
    containerToolbarLayout->addWidget(d->menuButton);
    containerToolbarLayout->addWidget(d->closeContainerButton);

    d->toolBarLayout->addWidget(viewToolbarContainer);
    d->toolBarLayout->addWidget(containerToolbar,0,Qt::AlignRight);

    d->mainLayout = new QGridLayout(this);
    d->mainLayout->setContentsMargins(0, 0, 0, 0);
    d->mainLayout->setSpacing(0);
    d->mainLayout->addWidget(toolBar, 0, 0, Qt::AlignTop);
    d->mainLayout->addWidget(d->defaultWidget, 0, 0, 0, 0, Qt::AlignCenter);

    this->setAcceptDrops(true);
    this->setUserSplittable(true);
    this->setClosingMode(medViewContainer::CLOSE_CONTAINER);
    this->setMultiLayered(true);
    this->setFocusPolicy(Qt::ClickFocus);
    this->setMouseTracking(true);
    this->setUserOpenable(true);

    d->selected = false;
    this->setSelected(false);

    d->defaultStyleSheet = this->styleSheet();
}

medViewContainer::~medViewContainer()
{
    removeInternView();

    // Trick to 'inform' a parented splitter
    // "you're not my dad anymore!"
    // There is no  'takeItem()' or 'removeWidget()' or wathever methode to remove a widget from a QSplitter.
    // this is used to remove the ownership of the container, If the parent splitter end up with no child it will be deleted.
    // see medViewContainerSplitter::~medViewContainerSplitter() and medViewContainerSplitter::checkIfStillDeserveToLive()
    this->setParent(NULL);

    medViewContainerManager::instance()->unregisterContainer(this);
    delete d;
}

void medViewContainer::popupMenu()
{
    QPoint pos = d->menuButton->mapToGlobal(QPoint(0, 0));
    pos.setY(pos.y() + d->menuButton->height());
    pos.setX(pos.x() + 2);
    d->toolBarMenu->popup(pos);
}

QUuid medViewContainer::uuid() const
{
    return d->uuid;
}

medAbstractView* medViewContainer::view() const
{
    return d->view;
}

QWidget* medViewContainer::defaultWidget() const
{
    return d->defaultWidget;
}

void medViewContainer::setDefaultWidget(QWidget *defaultWidget)
{
    if(!d->view)
    {
        d->mainLayout->removeWidget(d->defaultWidget);
        delete d->defaultWidget;
        d->mainLayout->addWidget(defaultWidget, 0, 0, 0, 0/*, Qt::AlignCenter*/); //otherwise defaultWidget doesn't use all the available space
    }
    d->defaultWidget = defaultWidget;
}

bool medViewContainer::isUserOpenable() const
{
    return d->userOpenable;
}

void medViewContainer::setUserOpenable(bool openable)
{
    d->userOpenable = openable;
    if(d->userOpenable)
    {
        this->setAcceptDrops(true);
        d->openAction->setEnabled(true);
    }
    else
    {
        this->setAcceptDrops(false);
        d->openAction->setEnabled(false);
    }
}

bool medViewContainer::isUserSplittable() const
{
    return d->userSplittable;
}

void medViewContainer::setUserSplittable(bool splittable)
{
    d->userSplittable = splittable;

    if(d->userSplittable)
    {
        d->hSplitAction->setEnabled(true);
        d->vSplitAction->setEnabled(true);
        d->presetMenu->setEnabled(true);

        medAbstractImageView *view = dynamic_cast <medAbstractImageView *> (d->view);
        if (view)
        {
            d->fourSplitAction->setEnabled(true);
        }
    }
    else
    {
        d->hSplitAction->setEnabled(false);
        d->vSplitAction->setEnabled(false);
        d->presetMenu->setEnabled(false);
        d->fourSplitAction->setEnabled(false);
    }
}

void medViewContainer::splitContainer(unsigned int numY, unsigned int numX)
{
    if (!this->isUserSplittable())
        return;

    medViewContainerSplitter *motherSplitter = dynamic_cast <medViewContainerSplitter *> (this->parent());
    if (!motherSplitter)
        return;

    QList <medViewContainer *> listSplitsX;
    listSplitsX << this;
    for (unsigned int i = 1;i < numX;++i)
        listSplitsX << this->splitVertically();

    for (unsigned int i = 1;i < numY;++i)
    {
        foreach(medViewContainer *container, listSplitsX)
            container->splitHorizontally();
    }

    motherSplitter->adjustContainersSize();

    if(d->toolBarMenu->isVisible())
        d->toolBarMenu->hide();
}

medViewContainer::ClosingMode medViewContainer::closingMode() const
{
    return d->closingMode;
}

void medViewContainer::setClosingMode(medViewContainer::ClosingMode mode)
{
    d->closingMode = mode;
    switch(d->closingMode)
    {
    case medViewContainer::CLOSE_CONTAINER:
        d->closeContainerButton->show();
        d->closeContainerButton->disconnect(this, SLOT(removeView()));
        connect(d->closeContainerButton, SIGNAL(clicked()), this, SLOT(close()));
        break;
    case medViewContainer::CLOSE_VIEW:
        d->closeContainerButton->show();
        d->closeContainerButton->disconnect(this, SLOT(close()));
        connect(d->closeContainerButton, SIGNAL(clicked()), this, SLOT(removeView()));
        break;
    case medViewContainer::CLOSE_BUTTON_HIDDEN:
        d->closeContainerButton->hide();
        d->closeContainerButton->disconnect(this, SLOT(close()));
        connect(d->closeContainerButton, SIGNAL(clicked()), this, SLOT(removeView()));
        break;
    }
}

bool medViewContainer::isMultiLayered() const
{
    return d->multiLayer;
}

void medViewContainer::setMultiLayered(bool multilayer)
{
    d->multiLayer = multilayer;
}

void medViewContainer::setContainerParent(medViewContainerSplitter *splitter)
{
    d->parent = splitter;
}

void medViewContainer::setView(medAbstractView *view)
{
    if(d->view)
    {
        d->view->viewWidget()->hide();
        this->removeInternView();
    }
    if(view)
    {
        d->view = view;
        connect(d->view, SIGNAL(destroyed()), this, SLOT(removeInternView()));
        connect(d->view, SIGNAL(selectedRequest(bool)), this, SLOT(setSelected(bool)));

        if(medAbstractLayeredView* layeredView = dynamic_cast<medAbstractLayeredView*>(view))
        {
            connect(layeredView, SIGNAL(currentLayerChanged()), this, SIGNAL(currentLayerChanged()));
            connect(layeredView, SIGNAL(currentLayerChanged()), this, SLOT(updateToolBar()));
            connect(layeredView, SIGNAL(layerAdded(uint)), this, SIGNAL(viewContentChanged()));
            connect(layeredView, SIGNAL(layerRemoved(uint)), this, SIGNAL(viewContentChanged()));
        }

        if (medAbstractImageView * imageView = dynamic_cast<medAbstractImageView*>(view))
        {
            connect(d->histogramAction, SIGNAL(toggled(bool)), this, SLOT(toggleHistogram(bool)));
            connect(d->histogramAction, SIGNAL(toggled(bool)), imageView, SLOT(showHistogram(bool)));

            d->histogramAction->setEnabled(true);
            if (d->userSplittable)
            {
                d->fourSplitAction->setEnabled(true);
                connect(this,SIGNAL(requestFourSplit()),imageView,SLOT(switchToFourViews()));
            }
        }

        d->saveSceneAction->setEnabled(true);
        d->defaultWidget->hide();
        d->mainLayout->addWidget(d->view->viewWidget(), 2, 0, 1, 1);
        d->view->viewWidget()->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
        d->view->viewWidget()->show();

        emit viewChanged();
    }
}

bool medViewContainer::isSelected() const
{
    return d->selected;
}

void medViewContainer::setSelected(bool selec)
{
    if(selec == d->selected)
    {
        if(QApplication::keyboardModifiers() == Qt::ControlModifier)
        {
            this->setSelected(!selec);
        }
        //clear focus in order to select/unselect successively twice the same container
        //this->clearFocus();
        return;
    }

    d->selected = selec;
    if(d->selected)
    {
        emit containerSelected(d->uuid);
    }
    else
    {
        emit containerUnSelected(d->uuid);
        this->unHighlight();
    }
    //clear focus in order to select/unselect successively twice the same container
    //this->clearFocus();
}

void medViewContainer::highlight(QString color)
{
    // TODO: recomputeStyleSheet deosn't seem to work here
    // temporary setStyleSheet to update the border color
    QString styleSheet = "medViewContainer {border:1px solid " + color + ";}";
    this->setStyleSheet(styleSheet);
    if(d->view)
    {
        d->view->viewWidget()->updateGeometry();
        d->view->viewWidget()->update();
    }

    d->highlightColor = color;
}

void medViewContainer::unHighlight()
{
    this->setStyleSheet("medViewContainer {border:1px solid #909090;}");
    if(d->view)
    {
        d->view->viewWidget()->updateGeometry();
        d->view->viewWidget()->update();
    }
}

void medViewContainer::setUnSelected(bool unSelected)
{
    this->setSelected(!unSelected);
}

void medViewContainer::toggleMaximized(bool checked)
{
    if(checked)
    {
        d->vSplitAction->setEnabled(false);
        d->hSplitAction->setEnabled(false);
        d->fourSplitAction->setEnabled(false);
        d->presetMenu->setEnabled(false);
        d->closeContainerButton->setEnabled(false);
        d->maximizedAction->setText("Unmaximize");
        d->maximizedAction->setChecked(true);
    }
    else
    {
        if(d->userSplittable)
        {
            d->vSplitAction->setEnabled(true);
            d->hSplitAction->setEnabled(true);
            d->fourSplitAction->setEnabled(true);
            d->presetMenu->setEnabled(true);
        }
        d->closeContainerButton->setEnabled(true);
        d->maximizedAction->setText("Maximize");
        d->maximizedAction->setChecked(false);
    }
    emit maximized(checked);
    emit maximized(d->uuid, checked);
}

bool medViewContainer::isMaximized() const
{
    return d->maximizedAction->isChecked();
}

void medViewContainer::toggleHistogram(bool checked)
{
    if (checked)
    {
        d->histogramAction->setText("Close Histogram");
    }
    else
    {
        d->histogramAction->setText("Open Histogram");
    }
}

void medViewContainer::removeView()
{
    if(!d->view)
        return;

    delete d->view;
    // removeInternView should be called, so no need to set d->view to NULL
    // or whatever else
}

void medViewContainer::removeInternView()
{
    d->view = NULL;
    d->maximizedAction->setEnabled(false);
    d->saveSceneAction->setEnabled(false);
    d->histogramAction->setEnabled(false);
    d->defaultWidget->show();
    this->updateToolBar();

    emit viewRemoved();
}

void medViewContainer::focusInEvent(QFocusEvent *event)
{
    if(event->reason() == Qt::ActiveWindowFocusReason)
        return;

    this->setSelected(true);
    QWidget::focusInEvent(event);
}

void medViewContainer::recomputeStyleSheet()
{
    this->style()->unpolish(this);
    this->style()->polish(this);
}

void medViewContainer::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void medViewContainer::dragMoveEvent(QDragMoveEvent *event)
{
    if(d->userSplittable)
    {
        DropArea area = computeDropArea(event->pos().x(), event->pos().y());
        switch(area) {
            case AREA_TOP:
                this->setStyleSheet("medViewContainer {border-top: 1px solid #0080FF}");
                break;
            case AREA_RIGHT:
                this->setStyleSheet("medViewContainer {border-right: 1px solid #0080FF}");
                break;
            case AREA_BOTTOM:
                this->setStyleSheet("medViewContainer {border-bottom: 1px solid #0080FF}");
                break;
            case AREA_LEFT:
                this->setStyleSheet("medViewContainer {border-left: 1px solid #0080FF}");
                break;
            case AREA_CENTER:
                this->setStyleSheet("medViewContainer {border: 1px solid #0080FF}");
                break;
            default:
                qDebug("Unsupported DropArea value");
        }
    }

    event->acceptProposedAction();
}

void medViewContainer::dragLeaveEvent(QDragLeaveEvent *event)
{
    this->setStyleSheet(d->defaultStyleSheet);
    if(d->selected)
        this->highlight(d->highlightColor);

    event->accept();
}

void medViewContainer::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    medDataIndex index = medDataIndex::readMimeData(mimeData);

    // User can drop a study or a series into the view
    if(index.isValidForSeries() || index.isValidForStudy())
    {
        if(d->userSplittable)
        {
            DropArea area = computeDropArea(event->pos().x(), event->pos().y());

            if(area == AREA_TOP)
                emit splitRequest(index, Qt::AlignTop);
            else if(area == AREA_RIGHT)
                emit splitRequest(index, Qt::AlignRight);
            else if(area == AREA_BOTTOM)
                emit splitRequest(index, Qt::AlignBottom);
            else if(area == AREA_LEFT)
                emit splitRequest(index, Qt::AlignLeft);
            else if(area == AREA_CENTER)
                this->addData(index);
        }
        else
        {
            this->addData(index);
        }

        this->setStyleSheet(d->defaultStyleSheet);
        if(d->selected)
            this->highlight(d->highlightColor);

        event->acceptProposedAction();
    }
}

void medViewContainer::addData(medAbstractData *data)
{
    if( ! d->expectedUuids.isEmpty())
        return; // we're already waiting for a import to finish, don't accept other data

    if(!data)
        return;

    if(!d->multiLayer)
        this->removeView();

    if(!d->view)
    {
        //TODO find from data(factory?) which view have to be created - RDE
        medAbstractLayeredView* view;
        view = medViewFactory::instance()->createView<medAbstractLayeredView>("medVtkView", this);

        if(!view)
        {
            qWarning() << "medViewContainer: Unable to create a medVtkView";
            return;
        }
        this->setView(view);
    }
    //TODO bring a way to kow how to add the data to the view.
    //     assuming by now that we always have at least layered views.
    //     this can be did by having a method that return the base abstract class(categorie)
    //     of the view. - RDE
    medAbstractLayeredView* view = dynamic_cast<medAbstractLayeredView*>(d->view);
    view->addLayer(data);

    setSelected(true);
}

void medViewContainer::addData(medDataIndex index)
{
    if( ! d->expectedUuids.isEmpty())
        return; // we're already waiting for a import to finish, don't accept other data

    if (index.isValidForSeries())
    {
        this->addData(medDataManager::instance()->retrieveData(index));
    }
    else if (index.isValidForStudy())
    {
        // We get the list of each series from that study index, and open it
        QList<medDataIndex> seriesList = medDataManager::instance()->getSeriesListFromStudy(index);
        if (seriesList.count() > 0)
        {
            bool userIsOk = true;

            if (seriesList.count() > 10)
            {
                userIsOk = userValidationForStudyDrop();
            }

            if (userIsOk)
            {
                foreach (medDataIndex seriesIndex, seriesList)
                {
                    this->addData(medDataManager::instance()->retrieveData(seriesIndex));
                }
            }
        }
    }
}

bool medViewContainer::userValidationForStudyDrop()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Study Drop");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText("Are you sure you want to open this study?");
    msgBox.setInformativeText("This action is going to open every data from the study.");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    if (msgBox.exec() == QMessageBox::Yes)
    {
        return true;
    }
    return false;
}

medViewContainer * medViewContainer::splitHorizontally()
{
    if(!d->parent)
        return NULL;

    return d->parent->splitHorizontally(this);
}

medViewContainer * medViewContainer::splitVertically()
{
    if(!d->parent)
        return NULL;

    return d->parent->splitVertically(this);
}

medViewContainer *medViewContainer::split(Qt::AlignmentFlag alignement)
{
    if(!d->parent)
        return NULL;

    return d->parent->split(this, alignement);
}

void medViewContainer::closeEvent(QCloseEvent * /*event*/)
{
    // never delete an object in code that is called synchronously from the object's event handler
    this->deleteLater();
}

void medViewContainer::openFromSystem()
{
    //  get last directory opened in settings.
    QString path;
    QFileDialog dialog(this);

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.restoreState(medSettingsManager::instance()->value("state", "openFromSystem").toByteArray());
    dialog.restoreGeometry(medSettingsManager::instance()->value("geometry", "openFromSystem").toByteArray());
    if(dialog.exec())
        path = dialog.selectedFiles().first();

    medSettingsManager::instance()->setValue("state", "openFromSystem", dialog.saveState());
    medSettingsManager::instance()->setValue("geometry", "openFromSystem", dialog.saveGeometry());


    if (path.isEmpty())
        return;

    open(path);
}

void medViewContainer::open(const QString & path)
{
    QUuid uuid = medDataManager::instance()->importPath(path, false);
    d->expectedUuids.append(uuid);
    connect(medDataManager::instance(), SIGNAL(dataImported(medDataIndex,QUuid)),
            this, SLOT(open_waitForImportedSignal(medDataIndex,QUuid)));

    QEventLoop loop;
    connect(this, SIGNAL(importFinished()), &loop, SLOT(quit()));
    loop.exec();

    //  save last directory opened in settings.
    medSettingsManager::instance()->setValue("path", "medViewContainer", path);
}


void medViewContainer::open_waitForImportedSignal(medDataIndex index, QUuid uuid)
{
    if(d->expectedUuids.contains(uuid))
    {
        d->expectedUuids.removeAll(uuid);
        disconnect(medDataManager::instance(),SIGNAL(dataImported(medDataIndex,QUuid)),
                   this,SLOT(open_waitForImportedSignal(medDataIndex,QUuid)));
        if (index.isValid())
        {
            this->addData(index);
        }
        emit importFinished();
    }
}

void medViewContainer::updateToolBar()
{
    if(d->viewToolbar)
    {
        delete d->viewToolbar;
        d->viewToolbar = NULL;
    }

    if(d->view)
    {
        d->viewToolbar = d->view->toolBarWidget();

        if(d->viewToolbar)
        {
            d->viewToolbarContainerLayout->addWidget(d->viewToolbar);
        }
    }
    else
    {
        d->toolBarLayout->setStretch(0,1);
    }
}

medViewContainer::DropArea medViewContainer::computeDropArea(int x, int y)
{
    int w = this->width(), h = this->height();
    if (x >= w || x < 0 || y >= h || y < 0)
        return AREA_OUT;

    /*
     *                        top trigger zone
     *              +--------------------------------------+
     *              |\                                    /|
     *              | \ ________________________________ / |
     * Left trigger |  |                                |  | Right trigger
     *    zone      |  |________________________________|  |     zone
     *              | /scccccccccccccccccccccccccccccccce\ |
     *              |/ssccccccccccccccccccccccccccccccccee\|
     *              +--------------------------------------+
     *                       bottom trigger zone
     *
     * Each 'if' condition below is structured in the same way :
     * first we test if we are inside the concerned zone (ex. bottom trigger zone)
     * if yes, then we test whether we are inside one of the three parts of that
     * zone: the center part ("ccc"), or the correct side of either triangular
     * ends ("sss" or "eee").
     */

    int ltw = qMin(50,w/4), tth = qMin(50,h/4); // left trigger width, top trigger height
    int rtw = w - ltw, bth = h - tth;           // right trigger width, bottom trigger height

    if(x < ltw && ((y >= tth && y < bth) || (y < tth && y > x) || (y >= bth && (h-y) > x)))
        return AREA_LEFT;
    else if(x >= rtw && ((y >= tth && y < bth) || (y < tth && y > (w-x)) || (y >= bth && (h-y) > (w-x))))
        return AREA_RIGHT;
    else if(y < tth && ((x >= ltw && x < rtw) || (x < ltw && x > y) || (x >= rtw && (w-x) > y)))
        return AREA_TOP;
    else if(y >= bth && ((x >= ltw && x < rtw) || (x < ltw && x > (h-y)) || (x >= rtw && (w-x) > (h-y))))
        return AREA_BOTTOM;
    return AREA_CENTER;
}

void medViewContainer::addColorIndicator(QColor color, QString description)
{
    d->poolIndicator->addColorIndicator(color, description);
}

void medViewContainer::removeColorIndicator(QColor color)
{
    d->poolIndicator->removeColorIndicator(color);
}

/**
 * @brief saves the scene in a XML file
 * Saves views (all layers), and toolboxes parameters
 * 	expected tree is as follow:
 *  workingDir (user-defined)
 * 		|-viewID0
 * 			|-mapping.xml
 * 			|-layerID01.xml
 * 			|-layerID02.xml
 * 			|-data_file (as saved by the dedicated writer)
 */
QString medViewContainer::saveScene()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/home",QFileDialog::ShowDirsOnly);
    QDir workingDir(dirPath);
    QDomDocument doc("xml");
    QDomElement root=doc.createElement("scene");
    doc.appendChild(root);

    QString subDirName;

    medAbstractLayeredView* layeredView=dynamic_cast<medAbstractLayeredView*>(view());
    if (layeredView)
    {
        subDirName="view"+QUuid::createUuid ().toString();
        if(workingDir.mkdir(subDirName))
        {
            workingDir.cd(subDirName);
            QString generatedPath=workingDir.canonicalPath()+"/mapping.xml";
            QDomElement layeredViewInfo=doc.createElement("layeredView");
            layeredViewInfo.setAttribute("path",subDirName+"/mapping.xml");
            layeredViewInfo.setAttribute("id",layeredView->identifier());
            root.appendChild(layeredViewInfo);

            // Patient informations added in the xml file
            QDomElement patientInfo = doc.createElement("patient");

            medAbstractData* dataset = layeredView->layerData(0);

            addMetadataToQDomElement(dataset, patientInfo, medMetaDataKeys::PatientName.key());
            addMetadataToQDomElement(dataset, patientInfo, medMetaDataKeys::PatientID.key());
            addMetadataToQDomElement(dataset, patientInfo, medMetaDataKeys::BirthDate.key());
            addMetadataToQDomElement(dataset, patientInfo, medMetaDataKeys::Gender.key());
            addMetadataToQDomElement(dataset, patientInfo, medMetaDataKeys::StudyDescription.key());

            if (patientInfo.hasAttributes())
            {
                root.appendChild(patientInfo);
            }

            layeredView->write(generatedPath);
            workingDir.cdUp();

            QString generatedGlobalPath=workingDir.canonicalPath()+"/globalMapping.xml";

            QFile file(generatedGlobalPath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
            {
                QTextStream out(&file);
                out << doc.toString();
                QString path = dirPath + "/" + subDirName;
                return path;
            }
            else
            {
                displayMessageError("Couldn't open: " + generatedGlobalPath);
            }
        }
        else
        {
            displayMessageError("Failed to create new directory: " + subDirName);
        }
    }
    else
    {
        displayMessageError("Scene saving failed");
    }
    return "";
}

void medViewContainer::addMetadataToQDomElement(medAbstractData* data, QDomElement patientInfo, QString metadata)
{
    if (data->hasMetaData(metadata) && data->metadata(metadata) != "")
    {
        patientInfo.setAttribute(metadata, data->metadata(metadata));
    }
}

void medViewContainer::loadScene()
{
    //parsing the XML file describing the scene,
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/home",tr("XML files (*.xml)"));
    QFile file(fileName);
    QFileInfo fileInfo(file);
    QDir workingDir=fileInfo.dir();
    if(file.open(QIODevice::ReadOnly))
    {
        QDomDocument doc("xml");
        if(doc.setContent(&file))
        {
            //doc now contains the full XML tree
            QDomNodeList viewsNodes=doc.elementsByTagName("layeredView");

            for(int i=0;i<viewsNodes.size();i++)
            {
                QDomElement viewElement=viewsNodes.item(i).toElement();
                if(viewElement.isNull())
                {
                    printInConsole("Failed to open a view, unable to find element");
                    continue;
                }
                QString path=workingDir.canonicalPath()+"/"+viewElement.attribute("path");
                if(!QFile::exists(path))
                {
                    printInConsole("Failed to open a view: path " + path + " does not exist");
                    continue;
                }
                QFile mappingFile(path);
                QFileInfo mappingFileInfo(mappingFile);
                if(!mappingFile.open(QIODevice::ReadOnly ))
                {
                    printInConsole("Failed to open a view, unable to open mapping file");
                    continue;
                }
                QDomDocument viewInfo("xml");
                if(viewInfo.setContent(&mappingFile))
                {
                    QDomNodeList layersNodes=viewInfo.elementsByTagName("layer");

                    for(int j=0;j<layersNodes.size();j++)
                    {
                        QDomElement layerElement=layersNodes.item(j).toElement();
                        QString fileName=layerElement.attribute("filename");
                        open(mappingFileInfo.dir().canonicalPath()+"/"+fileName);

                        medAbstractView *view = this->view();
                        view->restoreState(&layerElement);
                    }
                }
                else
                {
                     displayMessageError("Failed to parse " + fileName);
                     return;
                }
            }
        }
        else
        {
             displayMessageError("Failed to parse " + fileName);
        }
    }
    else
    {
         displayMessageError("Failed to open file " + fileName);
    }
}

void medViewContainer::printInConsole(QString message)
{
    qDebug() << Q_FUNC_INFO << ": " << message;
}

void medViewContainer::displayMessageError(QString message)
{
    printInConsole(message);
    medMessageController::instance()->showError(message,3000);
}
