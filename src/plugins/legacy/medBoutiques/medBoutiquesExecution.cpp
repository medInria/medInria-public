#include <regex>
#include <QtWidgets>
#include "medBoutiquesExecution.h"

medBoutiquesExecution::medBoutiquesExecution(QWidget *parent, medBoutiquesSearchTools *searchToolsWidget, medBoutiquesInvocation *invocationWidget) :
    QWidget(parent), searchToolsWidget(searchToolsWidget), invocationWidget(invocationWidget)
{
    connect(this->invocationWidget->invocationGUIWidget, &medBoutiquesInvocationGUI::invocationChanged, this, &medBoutiquesExecution::invocationChanged);
    this->layout = new QVBoxLayout();

    this->generatedCommandLabel = new QLabel("Generated command:");
    this->generatedCommand = new QTextEdit();
    this->executeButton = new QPushButton("Execute tool");
    this->cancelButton = new QPushButton("Cancel execution");
    this->cancelButton->hide();

    this->output = new QTextEdit();
    this->output->setReadOnly(true);

    this->layout->addWidget(this->generatedCommandLabel);
    this->layout->addWidget(this->generatedCommand);
    this->layout->addWidget(this->executeButton);
    this->layout->addWidget(this->cancelButton);
    this->layout->addWidget(this->output);

    connect(this->executeButton, &QPushButton::clicked, this, &medBoutiquesExecution::executeTool);
    connect(this->cancelButton, &QPushButton::clicked, this, &medBoutiquesExecution::cancelExecution);

    this->executionProcess = new QProcess(this);
    connect(this->executionProcess, &QProcess::errorOccurred, this, &medBoutiquesExecution::errorOccurred);
    connect(this->executionProcess, &QProcess::readyRead, this, &medBoutiquesExecution::dataReady);
    connect(this->executionProcess, &QProcess::started, this, &medBoutiquesExecution::executionProcessStarted);
    connect(this->executionProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &medBoutiquesExecution::executionProcessFinished);

    this->simulationProcess = new QProcess(this);
    connect(this->simulationProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &medBoutiquesExecution::simulationProcessFinished);

    this->setLayout(this->layout);
}

QString medBoutiquesExecution::getTemporaryInvocationFile()
{
    QDir temp = QDir::tempPath();
    temp.filePath("invocation.json");

    QString temporaryInvocationFilePath = temp.absoluteFilePath("invocation.json");
    QFile file(temporaryInvocationFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return "";

    file.write(this->invocationWidget->invocationEditor->toPlainText().toUtf8());
    return temporaryInvocationFilePath;
}

void medBoutiquesExecution::invocationChanged()
{
    SearchResult *tool = this->searchToolsWidget->getSelectedTool();

    if(tool == nullptr)
    {
        return;
    }
    QString temporaryInvocationFilePath = this->getTemporaryInvocationFile();

    this->simulationProcess->kill();
    this->simulationProcess->start(BOSH_PATH, {"exec", "simulate", "-i", temporaryInvocationFilePath.toStdString().c_str(), tool->id.c_str()});
}

void medBoutiquesExecution::simulationProcessFinished()
{
    QString output = QString::fromUtf8(this->simulationProcess->readAll());
    this->generatedCommand->setText(output);
}

void medBoutiquesExecution::executeTool()
{
    SearchResult *tool = this->searchToolsWidget->getSelectedTool();

    if(tool == nullptr)
    {
        return;
    }

    QString temporaryInvocationFilePath = this->getTemporaryInvocationFile();

    this->executionProcess->kill();
    this->executionProcess->start(BOSH_PATH, {"exec", "launch", "-s", tool->id.c_str(), temporaryInvocationFilePath.toStdString().c_str()});
    this->output->clear();
}

void medBoutiquesExecution::cancelExecution()
{
    this->executionProcess->kill();
    this->executeButton->show();
    this->cancelButton->show();
}

void medBoutiquesExecution::print(const QString& text)
{
    QTextCursor cursor = this->output->textCursor();
    cursor.movePosition(cursor.End);
    cursor.insertText(text);
    this->output->ensureCursorVisible();
}

void medBoutiquesExecution::executionProcessStarted()
{
    this->print("Process started...\n\n");
    this->executeButton->hide();
    this->cancelButton->show();
}

void medBoutiquesExecution::executionProcessFinished()
{
    this->print("Process finished.");
    this->executeButton->show();
    this->cancelButton->hide();
}

void medBoutiquesExecution::dataReady()
{
    QString output = QString::fromUtf8(this->executionProcess->readAll());

    regex e("\x1b\[[0-9;]*[mGKF]");

    string outputClean = regex_replace(output.toStdString(), e, "");
    this->print(QString::fromStdString(outputClean));
}

void medBoutiquesExecution::errorOccurred(QProcess::ProcessError error)
{
    Q_UNUSED(error)
    this->print("An error occured during the process execution.\n\n");
}

void medBoutiquesExecution::toolSelected()
{
    this->show();
}

void medBoutiquesExecution::toolDeselected()
{
    this->hide();
}
