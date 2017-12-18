/****************************************************************************
** Meta object code from reading C++ file 'WalkInPlaceTabController.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/tabcontrollers/WalkInPlaceTabController.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'WalkInPlaceTabController.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_walkinplace__WalkInPlaceTabController_t {
    QByteArrayData data[62];
    char stringdata0[954];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_walkinplace__WalkInPlaceTabController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_walkinplace__WalkInPlaceTabController_t qt_meta_stringdata_walkinplace__WalkInPlaceTabController = {
    {
QT_MOC_LITERAL(0, 0, 37), // "walkinplace::WalkInPlaceTabCo..."
QT_MOC_LITERAL(1, 38, 18), // "deviceCountChanged"
QT_MOC_LITERAL(2, 57, 0), // ""
QT_MOC_LITERAL(3, 58, 11), // "deviceCount"
QT_MOC_LITERAL(4, 70, 17), // "deviceInfoChanged"
QT_MOC_LITERAL(5, 88, 5), // "index"
QT_MOC_LITERAL(6, 94, 26), // "walkInPlaceProfilesChanged"
QT_MOC_LITERAL(7, 121, 19), // "enableStepDetection"
QT_MOC_LITERAL(8, 141, 6), // "enable"
QT_MOC_LITERAL(9, 148, 13), // "setStepIntSec"
QT_MOC_LITERAL(10, 162, 5), // "value"
QT_MOC_LITERAL(11, 168, 15), // "setHMDThreshold"
QT_MOC_LITERAL(12, 184, 2), // "xz"
QT_MOC_LITERAL(13, 187, 1), // "y"
QT_MOC_LITERAL(14, 189, 17), // "setAccuracyButton"
QT_MOC_LITERAL(15, 207, 8), // "buttonId"
QT_MOC_LITERAL(16, 216, 15), // "setHMDPitchDown"
QT_MOC_LITERAL(17, 232, 13), // "setHMDPitchUp"
QT_MOC_LITERAL(18, 246, 20), // "setHandWalkThreshold"
QT_MOC_LITERAL(19, 267, 13), // "walkThreshold"
QT_MOC_LITERAL(20, 281, 19), // "setHandJogThreshold"
QT_MOC_LITERAL(21, 301, 12), // "jogThreshold"
QT_MOC_LITERAL(22, 314, 19), // "setHandRunThreshold"
QT_MOC_LITERAL(23, 334, 12), // "runThreshold"
QT_MOC_LITERAL(24, 347, 12), // "setWalkTouch"
QT_MOC_LITERAL(25, 360, 11), // "setJogTouch"
QT_MOC_LITERAL(26, 372, 11), // "setRunTouch"
QT_MOC_LITERAL(27, 384, 15), // "setGameStepType"
QT_MOC_LITERAL(28, 400, 8), // "gameType"
QT_MOC_LITERAL(29, 409, 16), // "setControlSelect"
QT_MOC_LITERAL(30, 426, 7), // "control"
QT_MOC_LITERAL(31, 434, 21), // "addWalkInPlaceProfile"
QT_MOC_LITERAL(32, 456, 4), // "name"
QT_MOC_LITERAL(33, 461, 23), // "applyWalkInPlaceProfile"
QT_MOC_LITERAL(34, 485, 24), // "deleteWalkInPlaceProfile"
QT_MOC_LITERAL(35, 510, 14), // "getDeviceCount"
QT_MOC_LITERAL(36, 525, 15), // "getDeviceSerial"
QT_MOC_LITERAL(37, 541, 11), // "getDeviceId"
QT_MOC_LITERAL(38, 553, 14), // "getDeviceClass"
QT_MOC_LITERAL(39, 568, 14), // "getDeviceState"
QT_MOC_LITERAL(40, 583, 13), // "getDeviceMode"
QT_MOC_LITERAL(41, 597, 13), // "getStepIntSec"
QT_MOC_LITERAL(42, 611, 11), // "getGameType"
QT_MOC_LITERAL(43, 623, 16), // "getControlSelect"
QT_MOC_LITERAL(44, 640, 17), // "getAccuracyButton"
QT_MOC_LITERAL(45, 658, 15), // "getHMDPitchDown"
QT_MOC_LITERAL(46, 674, 13), // "getHMDPitchUp"
QT_MOC_LITERAL(47, 688, 17), // "getHMDXZThreshold"
QT_MOC_LITERAL(48, 706, 16), // "getHMDYThreshold"
QT_MOC_LITERAL(49, 723, 20), // "getHandWalkThreshold"
QT_MOC_LITERAL(50, 744, 19), // "getHandJogThreshold"
QT_MOC_LITERAL(51, 764, 19), // "getHandRunThreshold"
QT_MOC_LITERAL(52, 784, 12), // "getWalkTouch"
QT_MOC_LITERAL(53, 797, 11), // "getJogTouch"
QT_MOC_LITERAL(54, 809, 11), // "getRunTouch"
QT_MOC_LITERAL(55, 821, 22), // "isStepDetectionEnabled"
QT_MOC_LITERAL(56, 844, 14), // "isStepDetected"
QT_MOC_LITERAL(57, 859, 13), // "getGraphPoses"
QT_MOC_LITERAL(58, 873, 12), // "QList<qreal>"
QT_MOC_LITERAL(59, 886, 14), // "setupStepGraph"
QT_MOC_LITERAL(60, 901, 26), // "getWalkInPlaceProfileCount"
QT_MOC_LITERAL(61, 928, 25) // "getWalkInPlaceProfileName"

    },
    "walkinplace::WalkInPlaceTabController\0"
    "deviceCountChanged\0\0deviceCount\0"
    "deviceInfoChanged\0index\0"
    "walkInPlaceProfilesChanged\0"
    "enableStepDetection\0enable\0setStepIntSec\0"
    "value\0setHMDThreshold\0xz\0y\0setAccuracyButton\0"
    "buttonId\0setHMDPitchDown\0setHMDPitchUp\0"
    "setHandWalkThreshold\0walkThreshold\0"
    "setHandJogThreshold\0jogThreshold\0"
    "setHandRunThreshold\0runThreshold\0"
    "setWalkTouch\0setJogTouch\0setRunTouch\0"
    "setGameStepType\0gameType\0setControlSelect\0"
    "control\0addWalkInPlaceProfile\0name\0"
    "applyWalkInPlaceProfile\0"
    "deleteWalkInPlaceProfile\0getDeviceCount\0"
    "getDeviceSerial\0getDeviceId\0getDeviceClass\0"
    "getDeviceState\0getDeviceMode\0getStepIntSec\0"
    "getGameType\0getControlSelect\0"
    "getAccuracyButton\0getHMDPitchDown\0"
    "getHMDPitchUp\0getHMDXZThreshold\0"
    "getHMDYThreshold\0getHandWalkThreshold\0"
    "getHandJogThreshold\0getHandRunThreshold\0"
    "getWalkTouch\0getJogTouch\0getRunTouch\0"
    "isStepDetectionEnabled\0isStepDetected\0"
    "getGraphPoses\0QList<qreal>\0setupStepGraph\0"
    "getWalkInPlaceProfileCount\0"
    "getWalkInPlaceProfileName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_walkinplace__WalkInPlaceTabController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      46,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  244,    2, 0x06 /* Public */,
       4,    1,  247,    2, 0x06 /* Public */,
       6,    0,  250,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,  251,    2, 0x0a /* Public */,
       9,    1,  254,    2, 0x0a /* Public */,
      11,    2,  257,    2, 0x0a /* Public */,
      14,    1,  262,    2, 0x0a /* Public */,
      16,    1,  265,    2, 0x0a /* Public */,
      17,    1,  268,    2, 0x0a /* Public */,
      18,    1,  271,    2, 0x0a /* Public */,
      20,    1,  274,    2, 0x0a /* Public */,
      22,    1,  277,    2, 0x0a /* Public */,
      24,    1,  280,    2, 0x0a /* Public */,
      25,    1,  283,    2, 0x0a /* Public */,
      26,    1,  286,    2, 0x0a /* Public */,
      27,    1,  289,    2, 0x0a /* Public */,
      29,    1,  292,    2, 0x0a /* Public */,
      31,    1,  295,    2, 0x0a /* Public */,
      33,    1,  298,    2, 0x0a /* Public */,
      34,    1,  301,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
      35,    0,  304,    2, 0x02 /* Public */,
      36,    1,  305,    2, 0x02 /* Public */,
      37,    1,  308,    2, 0x02 /* Public */,
      38,    1,  311,    2, 0x02 /* Public */,
      39,    1,  314,    2, 0x02 /* Public */,
      40,    1,  317,    2, 0x02 /* Public */,
      41,    0,  320,    2, 0x02 /* Public */,
      42,    0,  321,    2, 0x02 /* Public */,
      43,    0,  322,    2, 0x02 /* Public */,
      44,    0,  323,    2, 0x02 /* Public */,
      45,    0,  324,    2, 0x02 /* Public */,
      46,    0,  325,    2, 0x02 /* Public */,
      47,    0,  326,    2, 0x02 /* Public */,
      48,    0,  327,    2, 0x02 /* Public */,
      49,    0,  328,    2, 0x02 /* Public */,
      50,    0,  329,    2, 0x02 /* Public */,
      51,    0,  330,    2, 0x02 /* Public */,
      52,    0,  331,    2, 0x02 /* Public */,
      53,    0,  332,    2, 0x02 /* Public */,
      54,    0,  333,    2, 0x02 /* Public */,
      55,    0,  334,    2, 0x02 /* Public */,
      56,    0,  335,    2, 0x02 /* Public */,
      57,    0,  336,    2, 0x02 /* Public */,
      59,    0,  337,    2, 0x02 /* Public */,
      60,    0,  338,    2, 0x02 /* Public */,
      61,    1,  339,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt,    3,
    QMetaType::Void, QMetaType::UInt,    5,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Float,   10,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   12,   13,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Float,   19,
    QMetaType::Void, QMetaType::Float,   21,
    QMetaType::Void, QMetaType::Float,   23,
    QMetaType::Void, QMetaType::Float,   10,
    QMetaType::Void, QMetaType::Float,   10,
    QMetaType::Void, QMetaType::Float,   10,
    QMetaType::Void, QMetaType::Int,   28,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, QMetaType::QString,   32,
    QMetaType::Void, QMetaType::UInt,    5,
    QMetaType::Void, QMetaType::UInt,    5,

 // methods: parameters
    QMetaType::UInt,
    QMetaType::QString, QMetaType::UInt,    5,
    QMetaType::UInt, QMetaType::UInt,    5,
    QMetaType::Int, QMetaType::UInt,    5,
    QMetaType::Int, QMetaType::UInt,    5,
    QMetaType::Int, QMetaType::UInt,    5,
    QMetaType::Float,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Bool,
    QMetaType::Bool,
    0x80000000 | 58,
    QMetaType::Void,
    QMetaType::UInt,
    QMetaType::QString, QMetaType::UInt,    5,

       0        // eod
};

void walkinplace::WalkInPlaceTabController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WalkInPlaceTabController *_t = static_cast<WalkInPlaceTabController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deviceCountChanged((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 1: _t->deviceInfoChanged((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 2: _t->walkInPlaceProfilesChanged(); break;
        case 3: _t->enableStepDetection((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setStepIntSec((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 5: _t->setHMDThreshold((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 6: _t->setAccuracyButton((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->setHMDPitchDown((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->setHMDPitchUp((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->setHandWalkThreshold((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 10: _t->setHandJogThreshold((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 11: _t->setHandRunThreshold((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 12: _t->setWalkTouch((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 13: _t->setJogTouch((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 14: _t->setRunTouch((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 15: _t->setGameStepType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->setControlSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->addWalkInPlaceProfile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 18: _t->applyWalkInPlaceProfile((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 19: _t->deleteWalkInPlaceProfile((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 20: { uint _r = _t->getDeviceCount();
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 21: { QString _r = _t->getDeviceSerial((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 22: { uint _r = _t->getDeviceId((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 23: { int _r = _t->getDeviceClass((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 24: { int _r = _t->getDeviceState((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 25: { int _r = _t->getDeviceMode((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 26: { float _r = _t->getStepIntSec();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 27: { int _r = _t->getGameType();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 28: { int _r = _t->getControlSelect();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 29: { int _r = _t->getAccuracyButton();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 30: { int _r = _t->getHMDPitchDown();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 31: { int _r = _t->getHMDPitchUp();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 32: { float _r = _t->getHMDXZThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 33: { float _r = _t->getHMDYThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 34: { float _r = _t->getHandWalkThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 35: { float _r = _t->getHandJogThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 36: { float _r = _t->getHandRunThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 37: { float _r = _t->getWalkTouch();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 38: { float _r = _t->getJogTouch();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 39: { float _r = _t->getRunTouch();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 40: { bool _r = _t->isStepDetectionEnabled();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 41: { bool _r = _t->isStepDetected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 42: { QList<qreal> _r = _t->getGraphPoses();
            if (_a[0]) *reinterpret_cast< QList<qreal>*>(_a[0]) = std::move(_r); }  break;
        case 43: _t->setupStepGraph(); break;
        case 44: { uint _r = _t->getWalkInPlaceProfileCount();
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 45: { QString _r = _t->getWalkInPlaceProfileName((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WalkInPlaceTabController::*_t)(unsigned  );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WalkInPlaceTabController::deviceCountChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WalkInPlaceTabController::*_t)(unsigned  );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WalkInPlaceTabController::deviceInfoChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (WalkInPlaceTabController::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WalkInPlaceTabController::walkInPlaceProfilesChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject walkinplace::WalkInPlaceTabController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_walkinplace__WalkInPlaceTabController.data,
      qt_meta_data_walkinplace__WalkInPlaceTabController,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *walkinplace::WalkInPlaceTabController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *walkinplace::WalkInPlaceTabController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_walkinplace__WalkInPlaceTabController.stringdata0))
        return static_cast<void*>(const_cast< WalkInPlaceTabController*>(this));
    return QObject::qt_metacast(_clname);
}

int walkinplace::WalkInPlaceTabController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 46)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 46;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 46)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 46;
    }
    return _id;
}

// SIGNAL 0
void walkinplace::WalkInPlaceTabController::deviceCountChanged(unsigned  _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void walkinplace::WalkInPlaceTabController::deviceInfoChanged(unsigned  _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void walkinplace::WalkInPlaceTabController::walkInPlaceProfilesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
