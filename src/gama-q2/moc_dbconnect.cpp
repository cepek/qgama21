/****************************************************************************
** Meta object code from reading C++ file 'dbconnect.h'
**
** Created: Sat Nov 3 15:02:21 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dbconnect.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dbconnect.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DBConnect[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      33,   10,   10,   10, 0x08,
      56,   10,   10,   10, 0x08,
      95,   10,   10,   10, 0x08,
     126,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DBConnect[] = {
    "DBConnect\0\0input_data_open(bool)\0"
    "switchStackedWidgets()\0"
    "on_pushButton_OpenFileDialog_clicked()\0"
    "on_pushButton_Cancel_clicked()\0"
    "on_pushButton_Connect_clicked()\0"
};

void DBConnect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DBConnect *_t = static_cast<DBConnect *>(_o);
        switch (_id) {
        case 0: _t->input_data_open((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->switchStackedWidgets(); break;
        case 2: _t->on_pushButton_OpenFileDialog_clicked(); break;
        case 3: _t->on_pushButton_Cancel_clicked(); break;
        case 4: _t->on_pushButton_Connect_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DBConnect::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DBConnect::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DBConnect,
      qt_meta_data_DBConnect, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DBConnect::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DBConnect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DBConnect::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DBConnect))
        return static_cast<void*>(const_cast< DBConnect*>(this));
    return QWidget::qt_metacast(_clname);
}

int DBConnect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DBConnect::input_data_open(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE