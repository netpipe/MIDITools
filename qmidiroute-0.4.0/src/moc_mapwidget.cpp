/****************************************************************************
** Meta object code from reading C++ file 'mapwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mapwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mapwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MapWidget_t {
    QByteArrayData data[15];
    char stringdata0[180];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MapWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MapWidget_t qt_meta_stringdata_MapWidget = {
    {
QT_MOC_LITERAL(0, 0, 9), // "MapWidget"
QT_MOC_LITERAL(1, 10, 12), // "updateTypeIn"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 5), // "index"
QT_MOC_LITERAL(4, 30, 13), // "updateTypeOut"
QT_MOC_LITERAL(5, 44, 15), // "updateChOutMode"
QT_MOC_LITERAL(6, 60, 18), // "updateIndexOutMode"
QT_MOC_LITERAL(7, 79, 10), // "updateChIn"
QT_MOC_LITERAL(8, 90, 5), // "value"
QT_MOC_LITERAL(9, 96, 11), // "updateChOut"
QT_MOC_LITERAL(10, 108, 13), // "updateIndexIn"
QT_MOC_LITERAL(11, 122, 14), // "updateIndexOut"
QT_MOC_LITERAL(12, 137, 13), // "updateRangeIn"
QT_MOC_LITERAL(13, 151, 14), // "updateRangeOut"
QT_MOC_LITERAL(14, 166, 13) // "updatePortOut"

    },
    "MapWidget\0updateTypeIn\0\0index\0"
    "updateTypeOut\0updateChOutMode\0"
    "updateIndexOutMode\0updateChIn\0value\0"
    "updateChOut\0updateIndexIn\0updateIndexOut\0"
    "updateRangeIn\0updateRangeOut\0updatePortOut"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MapWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x0a /* Public */,
       4,    1,   72,    2, 0x0a /* Public */,
       5,    1,   75,    2, 0x0a /* Public */,
       6,    1,   78,    2, 0x0a /* Public */,
       7,    1,   81,    2, 0x0a /* Public */,
       9,    1,   84,    2, 0x0a /* Public */,
      10,    1,   87,    2, 0x0a /* Public */,
      11,    1,   90,    2, 0x0a /* Public */,
      12,    1,   93,    2, 0x0a /* Public */,
      13,    1,   96,    2, 0x0a /* Public */,
      14,    1,   99,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,

       0        // eod
};

void MapWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MapWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateTypeIn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateTypeOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->updateChOutMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->updateIndexOutMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->updateChIn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->updateChOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->updateIndexIn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->updateIndexOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->updateRangeIn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->updateRangeOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->updatePortOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MapWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_MapWidget.data,
    qt_meta_data_MapWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MapWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MapWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MapWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MapWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
