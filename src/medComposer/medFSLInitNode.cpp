// Copyright: INRIA

#include <dtkComposer>

#include "medFSLInitNode.h"

Q_DECLARE_METATYPE(QProcessEnvironment);

class medFSLInitNodePrivate
{
public:
    dtkComposerTransmitterReceiver< QString > pathRecv;
    dtkComposerTransmitterEmitter< QProcessEnvironment > envEmt;
};

medFSLInitNode::medFSLInitNode(void) : d(new medFSLInitNodePrivate())
{
    this->appendReceiver(&d->pathRecv);
    this->appendEmitter(&d->envEmt);
}

medFSLInitNode::~medFSLInitNode(void)
{
    delete d;
}

void medFSLInitNode::run(void)
{
    if (d->pathRecv.isEmpty())
    {
        qDebug() << Q_FUNC_INFO << "The input is not set. Aborting.";
        return;
    }
    else
    {
        QString path=d->pathRecv.data();
        if(!QFile::exists(path))
        {
            qWarning()<<Q_FUNC_INFO<<path<<" does not exists";
            return;
        }
        QDir::setCurrent(path);
        QProcessEnvironment pe=QProcessEnvironment::systemEnvironment();
        pe.insert("FSLDIR",path);
        pe.insert("FSLOUTPUTTYPE","NIFTI_GZ");
        pe.insert("FSLTCLSH",path+"/bin/fsltclsh");
        pe.insert("FSLWISH",path+"/bin/fslwish");
        pe.insert("FSLMULTIFILEQUIT","TRUE");

        d->envEmt.setData(pe);
        qDebug()<<"done";
    }

}

