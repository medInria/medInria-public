#include <iostream>
#include <regex>
#include <QtWidgets>
#include "medBoutiquesExecutionWidget.h"

#include "medBoutiquesAbstractFileHandler.h"

medBoutiquesExecutionWidget::medBoutiquesExecutionWidget(QWidget *parent, medBoutiquesSearchToolsWidget *searchToolsWidget, medBoutiquesInvocationWidget *invocationWidget) :
    QWidget(parent), searchToolsWidget(searchToolsWidget), invocationWidget(invocationWidget)
{
    // Create the execution GUI:
    //  - a text edit to display the generated  command which will be given to the boutiques tool
    //  - buttons to execute / cancel the exec process (when one is shown, the other is hidden)
    //  - a read only text edit to display the process output
    connect(this->invocationWidget->invocationGUIWidget, &medBoutiquesInvocationGUIWidget::invocationChanged, this, &medBoutiquesExecutionWidget::invocationChanged);
    this->layout = new QVBoxLayout();

    this->generatedCommandLabel = new QLabel("Generated command:");
    this->generatedCommand = new QTextEdit();
    this->executeButton = new QPushButton("Execute tool");
    this->cancelButton = new QPushButton("Cancel execution");
    this->cancelButton->hide();

    this->output = new QTextEdit();
    this->output->setMinimumHeight(300);
    this->output->setReadOnly(true);

    this->layout->addWidget(this->generatedCommandLabel);
    this->layout->addWidget(this->generatedCommand);
    this->layout->addWidget(this->executeButton);
    this->layout->addWidget(this->cancelButton);
    this->layout->addWidget(this->output);

    connect(this->executeButton, &QPushButton::clicked, this, &medBoutiquesExecutionWidget::executeTool);
    connect(this->cancelButton, &QPushButton::clicked, this, &medBoutiquesExecutionWidget::cancelExecution);

    // Create the execution and the simulation processes
    this->executionProcess = new QProcess(this);
    connect(this->executionProcess, &QProcess::errorOccurred, this, &medBoutiquesExecutionWidget::errorOccurred);
    connect(this->executionProcess, &QProcess::readyRead, this, &medBoutiquesExecutionWidget::dataReady);
    connect(this->executionProcess, &QProcess::started, this, &medBoutiquesExecutionWidget::executionProcessStarted);
    connect(this->executionProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &medBoutiquesExecutionWidget::executionProcessFinished);

    this->simulationProcess = new QProcess(this);
    connect(this->simulationProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &medBoutiquesExecutionWidget::simulationProcessFinished);

    this->setLayout(this->layout);
}

QString medBoutiquesExecutionWidget::getTemporaryInvocationFile()
{
    // Write the temporary invocation file if possible, return an empty path otherwise
    QDir temp = QDir::tempPath();
    temp.filePath("invocation.json");

    QString temporaryInvocationFilePath = temp.absoluteFilePath("invocation.json");
    QFile file(temporaryInvocationFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return "";

    file.write(this->invocationWidget->invocationEditor->toPlainText().toUtf8());
    return temporaryInvocationFilePath;
}

void medBoutiquesExecutionWidget::invocationChanged()
{
    if(this->simulationProcess->state() != QProcess::NotRunning)
    {
        // If the simulation is running: kill the process and retry invocationChanged()
        this->simulationProcess->kill();
        QTimer::singleShot(100, this, &medBoutiquesExecutionWidget::invocationChanged);
        return;
    }

    ToolDescription *tool = this->searchToolsWidget->getSelectedTool();
    if(tool == nullptr)
    {
        return;
    }

    // Launch the simulation to generate the command line of the boutiques tool: bosh exec simulate -i temporaryInvocationFile toolID
    QString temporaryInvocationFilePath = this->getTemporaryInvocationFile();
    this->simulationProcess->start(BoutiquesPaths::Python(), {BoutiquesPaths::Bosh(), "exec", "simulate", "-i", temporaryInvocationFilePath, tool->id});
}

void medBoutiquesExecutionWidget::simulationProcessFinished()
{
    QString output = QString::fromUtf8(this->simulationProcess->readAll());
    this->generatedCommand->setText(output);
}

void medBoutiquesExecutionWidget::executeTool()
{
    // If the execution is running: kill the process and retry executeTool()
    if(this->executionProcess->state() != QProcess::NotRunning)
    {
        this->executionProcess->kill();
        QTimer::singleShot(100, this, &medBoutiquesExecutionWidget::executeTool);
        return;
    }

    ToolDescription *tool = this->searchToolsWidget->getSelectedTool();
    if(tool == nullptr)
    {
        return;
    }

    // Get the input and output directories which must be mounted to make them accessible by docker
    // Also generate the output file name to open it in medInria once the process is finished
    QString currentPath = QDir::currentPath();
    QStringList directories;
    this->invocationWidget->setAndGetAbsoluteDirectories(directories);

    // Launch the boutiques tool process execution: bosh exec launch -s toolID temporaryInvocationFile
    QString temporaryInvocationFilePath = this->getTemporaryInvocationFile();

    QStringList args({BoutiquesPaths::Bosh(), "exec", "launch"});
#ifndef Q_OS_WIN
    // Streams stdout and stderr in real time during execution.
    // Does not work on windows (problem with the "chmod 755 script.sh" command)
    args.append("-s");
#endif
    args.append({tool->id, temporaryInvocationFilePath});

    for(const QString &directory: directories)
    {
        args.push_back("-v");
        args.push_back(directory + ":" + directory);
    }

    this->executionProcess->start(BoutiquesPaths::Python(), args);
    this->output->clear();

    QDir::setCurrent(currentPath);
}

void medBoutiquesExecutionWidget::cancelExecution()
{
    this->executionProcess->kill();
    this->executeButton->show();
    this->cancelButton->show();
}

void medBoutiquesExecutionWidget::print(const QString& text)
{
    QTextCursor cursor = this->output->textCursor();
    cursor.movePosition(cursor.End);
    cursor.insertText(text);
    this->output->ensureCursorVisible();
}

void medBoutiquesExecutionWidget::executionProcessStarted()
{
    this->print("Process started...\n\n");
    this->executeButton->hide();
    this->cancelButton->show();
}

void medBoutiquesExecutionWidget::executionProcessFinished()
{
    this->print("Process finished.");
    this->executeButton->show();
    this->cancelButton->hide();

    emit success(this->invocationWidget->invocationGUIWidget->getOutputFileName());
}

void medBoutiquesExecutionWidget::dataReady()
{
    QString output = QString::fromUtf8(this->executionProcess->readAll());

    regex e("\x1b\[[0-9;]*[mGKF]");

    string outputClean = regex_replace(output.toStdString(), e, "");
    this->print(QString::fromStdString(outputClean));
}

void medBoutiquesExecutionWidget::errorOccurred(QProcess::ProcessError error)
{
    Q_UNUSED(error)
    this->print("An error occured during the process execution.\n\n");
}

void medBoutiquesExecutionWidget::toolSelected()
{
    this->show();
}

void medBoutiquesExecutionWidget::toolDeselected()
{
    this->hide();
}
