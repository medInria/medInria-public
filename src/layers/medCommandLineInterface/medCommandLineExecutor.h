#pragma once

#include <QObject>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "medCommandLineInterfaceExport.h"


class MEDCOMMANDLINE_EXPORT medCommandLineExecutor : public QObject
{
    Q_OBJECT

public:
    medCommandLineExecutor();
    static void execute(QString key);

public:
    virtual void init()=0;
    virtual void run()=0;

protected:
    QCommandLineParser m_parser;
};
