#include "medCommandLineExecutor.h"
#include "medCommandLineExecutorFactory.h"

medCommandLineExecutor::medCommandLineExecutor()
{
    QCommandLineOption pluginOption(QStringList() << "p" << "plugin", "force using the plugin named <plugin>.", "plugin");
    QCommandLineOption noGuiOption (QStringList() << "no-gui" , "disable gui");
    QCommandLineOption aOption (QStringList() << "a" << "direct run");

    m_parser.addOption(pluginOption);
    m_parser.addOption(noGuiOption);
    m_parser.addOption(aOption);
}

void medCommandLineExecutor::execute(QString key)
{
    medCommandLineExecutor* exec=medCommandLineExecutorFactory::instance()->get(key);
    if(exec==NULL)
    {
        qDebug()<<key << " not found";
        return;
    }

    exec->init();
    exec->run();
}
