%ignore operator=;
%ignore operator>>;
%ignore operator<<;
%ignore operator==;
%ignore operator[];
%ignore operator!=;
%ignore operator*=;
%ignore operator/=;
%ignore operator bool;
%ignore operator int;
%ignore operator float;
%ignore operator double;
%ignore operator double*;

#undef Q_OBJECT
#define Q_OBJECT

#undef Q_INVOKABLE
#define Q_INVOKABLE

#undef QT_BEGIN_NAMESPACE
#define QT_BEGIN_NAMESPACE

#undef QT_END_NAMESPACE
#define QT_END_NAMESPACE

#undef Q_WIDGETS_EXPORT
#define Q_WIDGETS_EXPORT

#undef QDOC_PROPERTY
#define QDOC_PROPERTY(text)

#undef signals
#define signals public

#undef slots
#define slots

#undef Q_SLOTS
#define Q_SLOTS

#undef Q_SIGNALS
#define Q_SIGNALS public

#undef  Q_PROPERTY
#define Q_PROPERTY(type, mode)

#undef  Q_DECLARE_INTERFACE
#define Q_DECLARE_INTERFACE(IFace, IId)

#undef  Q_DECLARE_METATYPE
#define Q_DECLARE_METATYPE(type)

#undef Q_DECLARE_FLAGS
#define Q_DECLARE_FLAGS(Flags, Enum)

#undef Q_DECLARE_PRIVATE
#define Q_DECLARE_PRIVATE(Class)

#undef QT_DEPRECATED_X
#define QT_DEPRECATED_X(text)

#undef Q_PROPERTY
#define Q_PROPERTY(...)

#undef Q_PROPERTY
#define Q_PROPERTY(text)

#undef Q_DISABLE_COPY
#define Q_DISABLE_COPY(Class)

#undef Q_PRIVATE_SLOT
#define Q_PRIVATE_SLOT(d, signature)

#undef Q_DECLARE_OPERATORS_FOR_FLAGS
#define Q_DECLARE_OPERATORS_FOR_FLAGS(Flags)

#undef  DTKCORESUPPORT_EXPORT
#define DTKCORESUPPORT_EXPORT

#undef  DTK_DECLARE_PRIVATE
#define DTK_DECLARE_PRIVATE(type)

#undef  DTK_IMPLEMENT_PRIVATE
#define DTK_IMPLEMENT_PRIVATE(type, parent)

#undef  MEDCORELEGACY_EXPORT
#define MEDCORELEGACY_EXPORT

#undef  MEDPYTHON_EXPORT
#define MEDPYTHON_EXPORT
