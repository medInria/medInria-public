%feature("director") medAbstractWorkspaceLegacy;

%{
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QShortcut>
#include <QScrollArea>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <medAbstractWorkspaceLegacy.h>
%}

#define slots
#define signals private

%include <medCoreLegacyExport.h>

%rename(AbstractWorkspaceLegacy) medAbstractWorkspaceLegacy;

%include "medAbstractWorkspaceLegacy.h"
