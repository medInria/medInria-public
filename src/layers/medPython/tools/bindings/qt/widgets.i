%ignore QTextDocument::begin;
%ignore QTextDocument::end;
%ignore QTextDocument::findBlock;
%ignore QTextDocument::findBlockByNumber;
%ignore QTextDocument::findBlockByLineNumber;
%ignore QTextDocument::firstBlock;
%ignore QTextDocument::lastBlock;
%ignore qt_qscrollbarStyleOption;
%ignore QWidget::fontMetrics;
%ignore QWidget::fontInfo;
%ignore QWidget::locale;
%ignore QWidget::nativeEvent;

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
%}

%include "qtwidgets-config.h"
%include "qtwidgetsglobal.h"

%feature("director") QWidget;
%include "qwidget.h"

%include "qshortcut.h"

SIGNAL_NOARGS(QShortcut, activated)

%include "qkeysequence.h"
%include "qframe.h"
%include "qabstractslider.h"
%include "qscrollbar.h"
%include "qabstractscrollarea.h"

SIGNAL(QAbstractSlider, rangeChanged, int, int)

%include "qscrollarea.h"
%include "qlabel.h"
%include "qtextdocument.h"
%include "qtextedit.h"

%feature("director") QLineEdit;
%include "qlineedit.h"

SIGNAL_NOARGS(QLineEdit, returnPressed)
SIGNAL(QLineEdit, cursorPositionChanged, int, int)

%include "qlayout.h"
%include "qboxlayout.h"
%include "qmainwindow.h"
