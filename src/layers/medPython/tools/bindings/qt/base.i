%ignore QFlag;

%include "qglobal.h"
%include "qflags.h"

#undef QT_DEPRECATED_SINCE
#define QT_DEPRECATED_SINCE(...) 0

#undef QT_DEPRECATED
#define QT_DEPRECATED(x)

#undef QT_DEPRECATED_X
#define QT_DEPRECATED_X(x)

#undef Q_DECLARE_PRIVATE
#define Q_DECLARE_PRIVATE(x)

#undef Q_DECLARE_FLAGS
#define Q_DECLARE_FLAGS(x, y) typedef y x;

#undef Q_DECLARE_OPERATORS_FOR_FLAGS
#define Q_DECLARE_OPERATORS_FOR_FLAGS(x)

%include "qnamespace.h"
%include "qsize.h"
%include "qtgui-config.h"
%include "qtguiglobal.h"
%include "qcoreevent.h"
%include "qevent.h"
%include "qfont.h"
%include "qicon.h"
