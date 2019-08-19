#include <iostream>
#include <QtWidgets>
#include "medBoutiquesInvocationWidget.h"

medBoutiquesInvocationWidget::medBoutiquesInvocationWidget(QWidget *parent, medBoutiquesSearchToolsWidget *searchToolsWidget, medBoutiquesAbstractFileHandler *medBoutiquesFileHandler) :
    QWidget(parent),
    searchToolsWidget(searchToolsWidget)
{
    this->layout = new QVBoxLayout();

    this->invocationGUIWidget = new medBoutiquesInvocationGUIWidget(this, this->searchToolsWidget, medBoutiquesFileHandler);
    this->openInvocationButton = new QPushButton("Open invocation file");

    this->invocationEditor = new QTextEdit();
    this->invocationEditor->setMinimumHeight(300);
    this->saveInvocationButton = new QPushButton("Save invocation file");

    this->layout->addWidget(this->openInvocationButton);
    this->layout->addWidget(this->invocationGUIWidget);
    this->layout->addWidget(this->invocationEditor);
    this->layout->addWidget(this->saveInvocationButton);

    connect(this->openInvocationButton, &QPushButton::clicked, this, &medBoutiquesInvocationWidget::openInvocationFile);
    connect(this->saveInvocationButton, &QPushButton::clicked, this, &medBoutiquesInvocationWidget::saveInvocationFile);
    connect(this->invocationGUIWidget, &medBoutiquesInvocationGUIWidget::invocationChanged, this, &medBoutiquesInvocationWidget::invocationChanged);

    this->setLayout(this->layout);

    this->generateInvocationProcess = new QProcess(this);
    connect(this->generateInvocationProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &medBoutiquesInvocationWidget::generateInvocationProcessFinished);
}

void medBoutiquesInvocationWidget::setAndGetAbsoluteDirectories(QStringList &directories)
{
    QString invocationString = this->invocationEditor->toPlainText();
    QJsonObject invocationJsonAbsolutePath = QJsonDocument::fromJson(invocationString.toUtf8()).object();

    this->invocationGUIWidget->populateDirectoriesAndSetOutputFileName(invocationJsonAbsolutePath, directories);

    QJsonDocument document(invocationJsonAbsolutePath);
    QByteArray output = document.toJson(QJsonDocument::Indented);
    this->invocationEditor->setText(output);
}

void medBoutiquesInvocationWidget::generateInvocationFile()
{
    if(this->generateInvocationProcess->state() != QProcess::NotRunning)
    {
        this->generateInvocationProcess->kill();
        QTimer::singleShot(100, this, &medBoutiquesInvocationWidget::generateInvocationFile);
        return;
    }

    const ToolDescription* tool = this->searchToolsWidget->getSelectedTool();
    if(tool == nullptr)
    {
        return;
    }

    QStringList args({BoutiquesPaths::Bosh(), "example"});
    if(this->invocationGUIWidget->generateCompleteInvocation())
    {
        args.append("--complete");
    }
    args.append(tool->id);
    this->generateInvocationProcess->start(BoutiquesPaths::Python(), args);
}

void medBoutiquesInvocationWidget::generateInvocationProcessFinished()
{
    QByteArray result = this->generateInvocationProcess->readAll();
    this->invocationJSON = QJsonDocument::fromJson(result).object();
    this->invocationEditor->setText(QString::fromUtf8(result));
    this->invocationGUIWidget->parseDescriptor(&this->invocationJSON);
}

void medBoutiquesInvocationWidget::openInvocationFile()
{
    const QString& name = QFileDialog::getOpenFileName(this, "Open invocation file");

    QFile file(name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QByteArray text = file.readAll();
    this->invocationEditor->setText(text);
}

void medBoutiquesInvocationWidget::invocationChanged()
{
    QJsonDocument document(this->invocationJSON);
    QByteArray output = document.toJson(QJsonDocument::Indented);
    this->invocationEditor->setText(output);
}

void medBoutiquesInvocationWidget::saveInvocationFile()
{
    const QString& name = QFileDialog::getSaveFileName(this, "Save invocation file");
    QFile file(name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    file.write(this->invocationEditor->toPlainText().toUtf8());
}

void medBoutiquesInvocationWidget::toolSelected()
{
    this->show();
    this->generateInvocationFile();
}

void medBoutiquesInvocationWidget::toolDeselected()
{
    this->hide();
}
