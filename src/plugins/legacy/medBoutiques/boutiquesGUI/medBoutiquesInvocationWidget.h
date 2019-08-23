#ifndef MEDBOUTIQUESINVOCATIONWIDGET_H
#define MEDBOUTIQUESINVOCATIONWIDGET_H

#include <QWidget>
#include "medBoutiquesConfiguration.h"
#include "medBoutiquesSearchToolsWidget.h"
#include "medBoutiquesInvocationGUIWidget.h"
#include "medBoutiquesAbstractFileHandler.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class medBoutiquesInvocationWidget : public QWidget
{
    Q_OBJECT
public:
    medBoutiquesInvocationGUIWidget *invocationGUIWidget;
    QTextEdit *invocationEditor;

private:
    medBoutiquesSearchToolsWidget *searchToolsWidget;
    QVBoxLayout *layout;
    QPushButton *openInvocationButton;
    QPushButton *saveInvocationButton;
    QJsonObject invocationJSON;
    QProcess *generateInvocationProcess;

public:
    explicit medBoutiquesInvocationWidget(QWidget *parent = nullptr, medBoutiquesSearchToolsWidget *searchToolsWidget = nullptr, medBoutiquesAbstractFileHandler *medBoutiquesFileHandler = nullptr);

    // When executing a tool, Boutiques must tell Docker to mount the directories in which the data lies (-v option)
    // setAndGetAbsoluteDirectories populates the list of directories to mount
    // and sets the output file name (which will automatically opened in medInria at the end of the process)
    // from the output-files (and their path-templates property) in the tool descriptor
    // setAndGetAbsoluteDirectories is called from medBoutiquesExecutionWidget::executeTool(), it modifies the invocation editor with the new absolute paths
    void setAndGetAbsoluteDirectories(QStringList &directories);

private:
    // Generator an example invocation file from the command "bosh example";
    // If some optional inputs are set in the tool GUI, a complete invocation file is generated with the "--complete" argument
    // Otherwise, a minimal invocation is created
    void generateInvocationFile();

signals:

public slots:
    void generateInvocationProcessFinished();
    void openInvocationFile();
    void saveInvocationFile();
    void invocationChanged();
    void toolSelected();
    void toolDeselected();
};

#endif // MEDBOUTIQUESINVOCATIONWIDGET_H
