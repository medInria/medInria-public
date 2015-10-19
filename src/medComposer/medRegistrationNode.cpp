// Copyright: INRIA

#include <dtkComposer>

#include "medRegistrationNode.h"
#include "medAbstractImageData.h"
#include "medWidgets/medAbstractRegistrationToolBox.h"
#include "medCore.h"

class medRegistrationNodePrivate
{
public:

    dtkComposerTransmitterReceiver< medAbstractImageData* >     fixedImageRecv;
    dtkComposerTransmitterReceiver< medAbstractImageData* >     movingImageRecv;
    
    dtkComposerTransmitterEmitter< medAbstractImageData* >      imgEmt;
    dtkComposerTransmitterEmitter< QTransform >      transfoEmt;
};

medRegistrationNode::medRegistrationNode(void) : dtkComposerNodeObject<medAbstractRegistrationProcess>(), d(new medRegistrationNodePrivate())
{
    this->setFactory(medCore::registration::nonRigid::pluginFactory());

    this->appendReceiver(&d->movingImageRecv);
    this->appendReceiver(&d->fixedImageRecv);

    this->appendEmitter (&d->imgEmt);
    this->appendEmitter (&d->transfoEmt);
}

medRegistrationNode::~medRegistrationNode(void)
{
    delete d;
}

QWidget* medRegistrationNode::editor()
{
    m_widget=medCore::registration::nonRigid::widgetFactory().get(this->currentImplementation());
    medAbstractRegistrationToolbox* toolbox=dynamic_cast<medAbstractRegistrationToolbox*>(m_widget);
    if(!toolbox)
        return NULL;
    toolbox->setProcess(this->object());
    return toolbox;
}

void medRegistrationNode::run(void)
{
    if (d->fixedImageRecv.isEmpty() || d->movingImageRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {
        medAbstractRegistrationProcess* process = this->object();
        if(!process)
        {
            qDebug()<<"no implementation found";
            return;
        }
        qDebug()<<"setting inputs";
        process->setMovingImage(d->movingImageRecv.data());
        process->setFixedImage(d->fixedImageRecv.data());
        process->run();
        d->imgEmt.setData(process->transformedImage());
        
        qDebug()<<"registration done";
    }
}

