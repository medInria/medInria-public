/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposerArea.h>

#include <QtGui>
#include <QtWidgets>


#include <dtkComposer/dtkComposer.h>
#include <dtkComposer/dtkComposerWidget.h>
#include <dtkComposer/dtkComposerCompass.h>
#include <dtkComposer/dtkComposerControls.h>
#include <dtkComposer/dtkComposerEvaluator.h>
#include <dtkComposer/dtkComposerFactory.h>
#include <dtkComposer/dtkComposerNodeFactoryView.h>
#include <dtkComposer/dtkComposerGraph.h>
#include <dtkComposer/dtkComposerGraphView.h>
#include <dtkComposer/dtkComposerScene.h>
#include <dtkComposer/dtkComposerSceneModel.h>
#include <dtkComposer/dtkComposerSceneNodeEditor.h>
#include <dtkComposer/dtkComposerSceneView.h>
#include <dtkComposer/dtkComposerStack.h>
#include <dtkComposer/dtkComposerStackView.h>
#include <dtkComposer/dtkComposerView.h>

#include <dtkGuiSupport/dtkScreenMenu.h>
#include <dtkGuiSupport/dtkRecentFilesMenu.h>
#include <dtkGuiSupport/dtkSpacer.h>
#include <dtkGuiSupport/dtkSplitter.h>
#include <dtkGuiSupport/dtkViewManager.h>

#include <dtkCore/dtkCore.h>
#include <dtkCore/dtkCorePluginManager.h>

#include "medComposer/medComposerFactoryExtension.h"

class medComposerAreaPrivate
{

public:
    dtkComposerWidget *composer;
    dtkComposerControls *controls;
    dtkComposerNodeFactoryView *nodes;
    dtkComposerGraphView *graph;
    dtkComposerSceneModel *model;
    dtkComposerSceneNodeEditor *editor;
    dtkComposerSceneView *scene;
    dtkComposerStackView *stack;

public:
    dtkViewManager *view_manager;

public:
    QHBoxLayout* mainLayout;

public:
    dtkSplitter *inner;

public:
    QMenu *composition_menu;
    QAction *composition_open_action;
    QAction *composition_save_action;
    QAction *composition_saveas_action;
    QAction *composition_insert_action;
    QAction *composition_quit_action;

    QMenu *edit_menu;
    QAction *undo_action;
    QAction *redo_action;

    dtkRecentFilesMenu *recent_compositions_menu;


public:
    QPushButton *compo_button;
    QPushButton *distr_button;
    QPushButton *debug_button;
    QPushButton *view_button;

public:
    QString current_composition;

public:
    int wl;
    int wr;
    int wg;

public:
    medComposerArea *q;
};

// /////////////////////////////////////////////////////////////////
// medComposerArea
// /////////////////////////////////////////////////////////////////

medComposerArea::medComposerArea(QWidget *parent) : QWidget(parent), d(new medComposerAreaPrivate)
{

    d->q = this;
    d->wl = 0;
    d->wr = 0;

    // --

    // -- Elements

    // -- to be encupsulated within distributed layer

    // d->distributor = new dtkDistributor(this);
    // d->distributor->setVisible(false);

    //

    d->composer = new dtkComposerWidget;
       d->composer->view()->setBackgroundBrush(QBrush(QPixmap(":dtkVisualProgramming/pixmaps/dtkComposerScene-bg.png")));
       d->composer->view()->setCacheMode(QGraphicsView::CacheBackground);

       QScopedPointer<dtkComposerNodeFactoryExtension> extension(new medComposerFactoryExtension);
       d->composer->factory()->extend(extension.data());

       d->controls = NULL;

       d->editor = new dtkComposerSceneNodeEditor(this);
       d->editor->setScene(d->composer->scene());
       d->editor->setStack(d->composer->stack());
       d->editor->setGraph(d->composer->graph());

       d->model = new dtkComposerSceneModel(this);
       d->model->setScene(d->composer->scene());

       d->scene = new dtkComposerSceneView(this);
       d->scene->setScene(d->composer->scene());
       d->scene->setModel(d->model);

       d->stack = new dtkComposerStackView(this);
       d->stack->setStack(d->composer->stack());

       d->nodes = new dtkComposerNodeFactoryView(this);
       d->nodes->setFactory(d->composer->factory());

       d->graph = new dtkComposerGraphView(this);
       d->graph->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
       d->graph->setScene(d->composer->graph());
       d->graph->setVisible(false);
       d->graph->setBackgroundBrush(QBrush(QPixmap(":dtkVisualProgramming/pixmaps/dtkComposerGraphView-bg.png")));

       // -- Actions

       d->composition_open_action = new QAction("Open", this);
       d->composition_open_action->setShortcut(QKeySequence::Open);

       d->composition_save_action = new QAction("Save", this);
       d->composition_save_action->setShortcut(QKeySequence::Save);

       d->composition_saveas_action = new QAction("Save As...", this);
       d->composition_saveas_action->setShortcut(QKeySequence::SaveAs);

       d->composition_insert_action = new QAction("Insert", this);
       d->composition_insert_action->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_I);

       d->composition_quit_action = new QAction("Quit", this);
       d->composition_quit_action->setShortcut(QKeySequence::Quit);

       d->undo_action = d->composer->stack()->createUndoAction(this);
       d->undo_action->setShortcut(QKeySequence::Undo);

       d->redo_action = d->composer->stack()->createRedoAction(this);
       d->redo_action->setShortcut(QKeySequence::Redo);

       // -- Toolbar

       QToolBar *mainToolBar = new QToolBar();
       mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
       mainToolBar->setIconSize(QSize(32, 32));

       QAction *run_action = mainToolBar->addAction(QIcon(":dtkVisualProgramming/pixmaps/dtkCreatorToolbarButton_Run_Active.png"), "Run");
       run_action->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_R);

       QAction *step_action = mainToolBar->addAction(QIcon(":dtkVisualProgramming/pixmaps/dtkCreatorToolbarButton_Step_Active.png"), "Step");
       step_action->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_N);

       QAction *continue_action = mainToolBar->addAction(QIcon(":dtkVisualProgramming/pixmaps/dtkCreatorToolbarButton_Continue_Active.png"), "Cont");
       continue_action->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_C);

       QAction *next_action = mainToolBar->addAction(QIcon(":dtkVisualProgramming/pixmaps/dtkCreatorToolbarButton_Continue_Active.png"), "Next");
       next_action->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_T);

       QAction *stop_action = mainToolBar->addAction(QIcon(":dtkVisualProgramming/pixmaps/dtkCreatorToolbarButton_Stop_Active.png"), "Stop");
       stop_action->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_Period);

       QAction *reset_action = mainToolBar->addAction(QIcon(":dtkVisualProgramming/pixmaps/dtkCreatorToolbarButton_Reset_Active.png"), "Reset");
       reset_action->setShortcut(Qt::ControlModifier + Qt::ShiftModifier + Qt::Key_D);

       /*QFrame *buttons = new QFrame(this);
       buttons->setObjectName("dtkVisualProgrammingMainWindowSegmentedButtons");

       d->compo_button = new QPushButton("Composition", buttons);
       d->compo_button->setObjectName("dtkVisualProgrammingMainWindowSegmentedButtonLeft");
       d->compo_button->setFixedSize(75, 25);
       d->compo_button->setCheckable(true);
       d->compo_button->setChecked(true);

       d->distr_button = new QPushButton("Distribution", buttons);
       d->distr_button->setObjectName("dtkVisualProgrammingMainWindowSegmentedButtonMiddle");
       d->distr_button->setFixedSize(75, 25);
       d->distr_button->setCheckable(true);

       d->debug_button = new QPushButton("Debug", buttons);
       d->debug_button->setObjectName("dtkVisualProgrammingMainWindowSegmentedButtonRight");
       d->debug_button->setFixedSize(75, 25);
       d->debug_button->setCheckable(true);

       d->view_button = new QPushButton("View", buttons);
       d->view_button->setObjectName("dtkVisualProgrammingMainWindowSegmentedButtonRight");
       d->view_button->setFixedSize(75, 25);
       d->view_button->setCheckable(true);

       QButtonGroup *button_group = new QButtonGroup(this);
       button_group->setExclusive(true);
       button_group->addButton(d->compo_button);
       button_group->addButton(d->distr_button);
       button_group->addButton(d->debug_button);
       button_group->addButton(d->view_button);

       QHBoxLayout *buttons_layout = new QHBoxLayout(buttons);
       buttons_layout->setMargin(0);
       buttons_layout->setSpacing(11);
       buttons_layout->addWidget(d->compo_button);
       buttons_layout->addWidget(d->distr_button);
       buttons_layout->addWidget(d->debug_button);
       buttons_layout->addWidget(d->view_button);

       mainToolBar->addWidget(buttons);*/

       // -- Menus

       /*QMenuBar *menu_bar = new QMenuBar();

       d->recent_compositions_menu = new dtkRecentFilesMenu("Open recent...", this);

       d->composition_menu = menu_bar->addMenu("Composition");
       d->composition_menu->addAction(d->composition_open_action);
       d->composition_menu->addMenu(d->recent_compositions_menu);
       d->composition_menu->addAction(d->composition_save_action);
       d->composition_menu->addAction(d->composition_saveas_action);
       d->composition_menu->addSeparator();
       d->composition_menu->addAction(d->composition_insert_action);
       d->composition_menu->addSeparator();
       d->composition_menu->addAction(d->composition_quit_action);

       d->edit_menu = menu_bar->addMenu("Edit");
       d->edit_menu->addAction(d->composer->view()->searchAction());
       d->edit_menu->addSeparator();
       d->edit_menu->addAction(d->undo_action);
       d->edit_menu->addAction(d->redo_action);
       d->edit_menu->addSeparator();
       d->edit_menu->addAction(d->composer->scene()->flagAsBlueAction());
       d->edit_menu->addAction(d->composer->scene()->flagAsGrayAction());
       d->edit_menu->addAction(d->composer->scene()->flagAsGreenAction());
       d->edit_menu->addAction(d->composer->scene()->flagAsOrangeAction());
       d->edit_menu->addAction(d->composer->scene()->flagAsPinkAction());
       d->edit_menu->addAction(d->composer->scene()->flagAsRedAction());
       d->edit_menu->addAction(d->composer->scene()->flagAsYellowAction());
       d->edit_menu->addSeparator();
       d->edit_menu->addAction(d->composer->scene()->maskEdgesAction());
       d->edit_menu->addAction(d->composer->scene()->unmaskEdgesAction());


       dtkScreenMenu *screen_menu = new dtkScreenMenu("Screen",this);
       menu_bar->addMenu(screen_menu);

       QAction *showControlsAction = new QAction("Show controls", this);
       showControlsAction->setShortcut(QKeySequence(Qt::ShiftModifier + Qt::ControlModifier + Qt::AltModifier + Qt::Key_C));

       QMenu *window_menu = menu_bar->addMenu("Window");
       window_menu->addAction(showControlsAction);

       QMenu *debug_menu = menu_bar->addMenu("Debug");
       debug_menu->addAction(run_action);
       debug_menu->addAction(step_action);
       debug_menu->addAction(continue_action);
       debug_menu->addAction(next_action);
       debug_menu->addAction(stop_action);
       debug_menu->addAction(reset_action);*/

       // -- Connections

       connect(run_action, SIGNAL(triggered()), d->composer, SLOT(run()));
       connect(step_action, SIGNAL(triggered()), d->composer, SLOT(step()));
       connect(continue_action, SIGNAL(triggered()), d->composer, SLOT(cont()));
       connect(next_action, SIGNAL(triggered()), d->composer, SLOT(next()));
       connect(stop_action, SIGNAL(triggered()), d->composer, SLOT(stop()));
       connect(reset_action, SIGNAL(triggered()), d->composer, SLOT(reset()));

       /*connect(switchToCompoAction, SIGNAL(triggered()), this, SLOT(switchToCompo()));
       connect(switchToDstrbAction, SIGNAL(triggered()), this, SLOT(switchToDstrb()));
       connect(switchToDebugAction, SIGNAL(triggered()), this, SLOT(switchToDebug()));
       connect(switchToViewAction, SIGNAL(triggered()), this, SLOT(switchToView()));

       connect(showControlsAction, SIGNAL(triggered()), this, SLOT(showControls()));

       // connect(d->view_manager, SIGNAL(focused(dtkAbstratView *)), this, SLOT(onViewFocused(dtkAbstractView *)));

       connect(d->compo_button, SIGNAL(pressed()), this, SLOT(switchToCompo()));
       connect(d->distr_button, SIGNAL(pressed()), this, SLOT(switchToDstrb()));
       connect(d->debug_button, SIGNAL(pressed()), this, SLOT(switchToDebug()));
       connect(d->view_button, SIGNAL(pressed()), this, SLOT(switchToView()));

       connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
*/
       // -- Layout

       dtkSplitter *left = new dtkSplitter(this);
       left->setOrientation(Qt::Vertical);
       left->addWidget(d->nodes);
       // left->addWidget(d->distributor);
       //left->addWidget(d->view_manager);

       dtkSplitter *right = new dtkSplitter(this);
       right->setOrientation(Qt::Vertical);
       //right->addWidget(d->scene);
       right->addWidget(d->editor);
       //right->addWidget(d->stack);
       right->addWidget(d->composer->compass());

       d->inner = new dtkSplitter(this);
       d->inner->setOrientation(Qt::Horizontal);
       d->inner->addWidget(left);
       //d->inner->addWidget(d->graph);
       d->inner->addWidget(d->composer);
       d->inner->addWidget(right);
       d->inner->setStretchFactor(0,1);
       d->inner->setStretchFactor(1,5);
       d->inner->setStretchFactor(2,1);


       QHBoxLayout *b_layout = new QHBoxLayout;
       b_layout->setContentsMargins(0, 0, 0, 0);
       b_layout->setSpacing(0);

       QWidget *bottom = new QWidget(this);
       bottom->setLayout(b_layout);

       dtkSplitter *central = new dtkSplitter(this);
       central->setOrientation(Qt::Vertical);
       central->addWidget(d->inner);

       d->mainLayout=new QHBoxLayout();
       d->mainLayout->setSpacing(0);
       d->mainLayout->setMargin(0);
       d->mainLayout->addWidget(central);
       setLayout(d->mainLayout);
       this->layout()->setMenuBar(mainToolBar);

 }

medComposerArea::~medComposerArea(void)
{
    delete d;
}

