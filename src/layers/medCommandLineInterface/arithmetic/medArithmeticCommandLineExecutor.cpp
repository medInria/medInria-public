#include "medArithmeticCommandLineExecutor.h"


class medArithmeticCommandLineExecutorPrivate
{
public:
    QString outputText;
    QString pluginName;
    medAbstractArithmeticOperationProcess* process;
};



medArithmeticCommandLineExecutor::medArithmeticCommandLineExecutor():d(new medArithmeticCommandLineExecutorPrivate())
{

}

medArithmeticCommandLineExecutor::~medArithmeticCommandLineExecutor()
{
    delete d;
}

void medArithmeticCommandLineExecutor::init()
{
    qDebug()<<Q_FUNC_INFO<<__LINE__;

    QString rhsText,lhsText;

    QCommandLineOption rhsOption (QStringList() << "r" << "rhs" , "right operand <file>.", "file"     );
    QCommandLineOption lhsOption (QStringList() << "l" << "lhs" , "second operand <file>.", "file"     );
    QCommandLineOption outputOption(QStringList() << "o" << "output", "Write generated data into <file>.", "file");

    m_parser.addOption(lhsOption);
    m_parser.addOption(rhsOption);
    m_parser.addOption(outputOption);

    qDebug()<<Q_FUNC_INFO<<__LINE__;

    m_parser.process(*(QCoreApplication::instance()));

    qDebug()<<Q_FUNC_INFO<<__LINE__;

    if(m_parser.isSet("l"))
        lhsText=m_parser.value("l");
    if(m_parser.isSet("r"))
        rhsText=m_parser.value("r");
    if(m_parser.isSet("o"))
        d->outputText=m_parser.value("o");
    if(m_parser.isSet("p"))
        d->pluginName=m_parser.value("p");

    qDebug()<<Q_FUNC_INFO<<__LINE__;
    
    d->process=this->getProcess(d->pluginName);
    if (!d->process)
    {
        dtkWarn()<<"no corresponding plugin";
    }

    medAbstractImageData *lhs,*rhs;
    
    lhs=dynamic_cast<medAbstractImageData*>(medDataReaderWriter::read(lhsText));
    rhs=dynamic_cast<medAbstractImageData*>(medDataReaderWriter::read(rhsText));

    if(!rhs)
    {
        dtkWarn()<<"invalid input file: "<<rhsText;
    }
    if(!lhs)
    {
        dtkWarn()<<"invalid input file: "<<lhsText;
    }
    d->process->setInput1(rhs);
    d->process->setInput2(lhs);
}

void medArithmeticCommandLineExecutor::run()
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
