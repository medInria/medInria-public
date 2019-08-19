#ifndef MEDBOUTIQUESEXECUTIONWIDGET_H
#define MEDBOUTIQUESEXECUTIONWIDGET_H

#include <QWidget>
#include "medBoutiquesConfiguration.h"
#include "medBoutiquesSearchToolsWidget.h"
#include "medBoutiquesInvocationWidget.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class medBoutiquesExecutionWidget : public QWidget
{
    Q_OBJECT
public:
    medBoutiquesSearchToolsWidget *searchToolsWidget;
    medBoutiquesInvocationWidget *invocationWidget;

private:
    QLayout *layout;
    QLabel *generatedCommandLabel;
    QTextEdit *generatedCommand;
    QTextEdit *output;
    QPushButton *executeButton;
    QPushButton *cancelButton;
    QProcess *executionProcess;
    QProcess *simulationProcess;

public:
    explicit medBoutiquesExecutionWidget(QWidget *parent = nullptr, medBoutiquesSearchToolsWidget *searchToolsWidget = nullptr, medBoutiquesInvocationWidget *invocationWidget = nullptr);

private:
    QString getTemporaryInvocationFile();
    void print(const QString& text);

signals:
    void success(const QString outputFileName);

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

#endif // MEDBOUTIQUESEXECUTIONWIDGET_H
