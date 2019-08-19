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
    void setAndGetAbsoluteDirectories(QStringList &directories);

private:
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
