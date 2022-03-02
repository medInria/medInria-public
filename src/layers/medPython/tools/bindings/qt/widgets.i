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
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QShortcut>
#include <QSizePolicy>
#include <QTextDocument>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
%}

%include "qtwidgets-config.h"
%include "qtwidgetsglobal.h"

%include "qsizepolicy.h"

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
%include "qabstractbutton.h"

SIGNAL(QAbstractButton, clicked, bool)

%include "qpushbutton.h"
%include "qtextdocument.h"
%include "qtextedit.h"

%feature("director") QLineEdit;
%include "qlineedit.h"

SIGNAL_NOARGS(QLineEdit, returnPressed)
SIGNAL(QLineEdit, cursorPositionChanged, int, int)

%include "qlayout.h"
%include "qboxlayout.h"
%include "qmainwindow.h"
