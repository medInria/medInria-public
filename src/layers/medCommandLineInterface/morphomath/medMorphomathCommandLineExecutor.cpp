#include "medMorphomathCommandLineExecutor.h"


class medMorphomathCommandLineExecutorPrivate
{
public:
    QString outputText;
    QString pluginName;
    medAbstractMorphomathOperationProcess* process;
};



medMorphomathCommandLineExecutor::medMorphomathCommandLineExecutor():d(new medMorphomathCommandLineExecutorPrivate())
{

}

medMorphomathCommandLineExecutor::~medMorphomathCommandLineExecutor()
{
    delete d;
}

void medMorphomathCommandLineExecutor::init()
{
    qDebug()<<Q_FUNC_INFO<<__LINE__;

    QString inputText,radiusText;

    QCommandLineOption inputOption (QStringList() << "i" << "input" , "Read input data from <file>.", "file"     );
    QCommandLineOption outputOption(QStringList() << "o" << "output", "Write generated data into <file>.", "file");
    QCommandLineOption radiusOption(QStringList() << "r" << "radius", "Set the radius of the process to <radius>.", "radius");


    m_parser.addOption(inputOption);
    m_parser.addOption(outputOption);
    m_parser.addOption(radiusOption);

    qDebug()<<Q_FUNC_INFO<<__LINE__;

    m_parser.process(*(QCoreApplication::instance()));

    qDebug()<<Q_FUNC_INFO<<__LINE__;

    if(m_parser.isSet("i"))
        inputText=m_parser.value("i");
    if(m_parser.isSet("o"))
        d->outputText=m_parser.value("o");
    if(m_parser.isSet("r"))
        radiusText=m_parser.value("r");
    if(m_parser.isSet("p"))
        d->pluginName=m_parser.value("p");

    qDebug()<<Q_FUNC_INFO<<__LINE__;
    
    d->process=this->getProcess(d->pluginName);
    if (!d->process)
    {
        dtkWarn()<<"no corresponding plugin";
    }

    medAbstractImageData *input;
    
    input=dynamic_cast<medAbstractImageData*>(medDataReaderWriter::read(inputText));
    
    if(!input)
    {
        dtkWarn()<<"invalid input file: "<<inputText;
    }


    bool ok;
    int radius=radiusText.toInt(&ok);
    if(!ok)
    {
        dtkWarn()<<"invalid input radius: "<<radiusText;
    }
    d->process->setInput(input);
    d->process->kernelRadius()->setValue(radius);
}

void medMorphomathCommandLineExecutor::run()
{
    qDebug()<<Q_FUNC_INFO<<__LINE__;

    if(d->process==NULL)
    {
        dtkWarn()<<"no process set";
       return;
    }

    medAbstractJob::medJobExitStatus out= d->process->run();
    qDebug()<<"the output is: "<<out;

    if(!d->process->output())
        qDebug()<<"process output is NULL";
    bool written=medDataReaderWriter::write(d->outputText,d->process->output());
    if(!written)
        qDebug()<<"failed to write "<<d->outputText;
     qDebug()<<Q_FUNC_INFO<<__LINE__;

}
