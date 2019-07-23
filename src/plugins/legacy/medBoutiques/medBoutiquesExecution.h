#ifndef MEDBOUTIQUESEXECUTION_H
#define MEDBOUTIQUESEXECUTION_H

#include <QWidget>
#include "medBoutiquesSearchTools.h"
#include "medBoutiquesInvocation.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class medBoutiquesExecution : public QWidget
{
    Q_OBJECT
public:
    medBoutiquesSearchTools *searchToolsWidget;
    medBoutiquesInvocation *invocationWidget;
    QLayout *layout;
    QLabel *generatedCommandLabel;
    QTextEdit *generatedCommand;
    QTextEdit *output;
    QPushButton *executeButton;
    QPushButton *cancelButton;
    QProcess *executionProcess;
    QProcess *simulationProcess;

public:
    explicit medBoutiquesExecution(QWidget *parent = nullptr, medBoutiquesSearchTools *searchToolsWidget = nullptr, medBoutiquesInvocation *invocationWidget = nullptr);

private:
    QString getTemporaryInvocationFile();
    void print(const QString& text);

signals:

public slots:
    void invocationChanged();
    void executeTool();
    void cancelExecution();
    void errorOccurred(QProcess::ProcessError error);
    void dataReady();
    void executionProcessStarted();
    void executionProcessFinished();
    void simulationProcessFinished();
    void toolSelected();
    void toolDeselected();
};

#endif // MEDBOUTIQUESEXECUTION_H
