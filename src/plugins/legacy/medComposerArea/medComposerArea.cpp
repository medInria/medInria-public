/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medComposerArea.h>

#include <dtkComposerWidget.h>
#include <dtkComposerSceneNodeEditor.h>
#include <dtkComposerSceneView.h>
#include <dtkComposerStackView.h>
#include <dtkComposerNodeFactoryView.h>
#include <dtkComposerGraphView.h>
#include <dtkComposerSceneModel.h>
#include <dtkComposerView.h>
#include <dtkComposerFactory.h>
#include <dtkComposerGraph.h>


#include <QGridLayout>



class medComposerAreaPrivate
{
public:
    dtkComposerWidget *composerWidget;
    dtkComposerSceneNodeEditor *editor;
    dtkComposerSceneModel *model;
    dtkComposerSceneView *scene;
    dtkComposerStackView *stack;
    dtkComposerNodeFactoryView* nodes;
    dtkComposerGraphView* graph;
};


medComposerArea::medComposerArea(QWidget *parent): medAbstractArea(parent),
    d(new medComposerAreaPrivate)
{
    d->composerWidget = new dtkComposerWidget;
    QGridLayout *layout = new QGridLayout;

    layout->addWidget(d->composerWidget);
    this->setLayout(layout);


    d->composerWidget->view()->setCacheMode(QGraphicsView::CacheBackground);

    dtkComposerNodeFactoryExtension *extension(new medSubtractImageProcessComposerFactoryExtension);
    d->composerWidget->factory()->extend(extension);


    d->editor = new dtkComposerSceneNodeEditor;
    d->editor->setScene(d->composerWidget->scene());
    d->editor->setStack(d->composerWidget->stack());
    d->editor->setGraph(d->composerWidget->graph());

    d->model = new dtkComposerSceneModel;
    d->model->setScene(d->composerWidget->scene());

    d->scene = new dtkComposerSceneView;
    d->scene->setScene(d->composerWidget->scene());
    d->scene->setModel(d->model);

    d->stack = new dtkComposerStackView;
    d->stack->setStack(d->composerWidget->stack());

    d->nodes = new dtkComposerNodeFactoryView;
    d->nodes->setFactory(d->composerWidget->factory());

    d->graph = new dtkComposerGraphView;
    d->graph->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    d->graph->setScene(d->composerWidget->graph());
    d->graph->setVisible(false);

//    layout->addWidget(d->editor);
//    layout->addWidget(d->scene);
    layout->addWidget(d->nodes);
//    layout->addWidget(d->graph);

}

medComposerArea::~medComposerArea()
{
    delete d;
}

QString medComposerArea::title() const
{
    return "Composer";
}

QString medComposerArea::description() const
{
    return tr("Description of the composer area");
}
