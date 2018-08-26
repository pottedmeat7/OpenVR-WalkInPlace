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
    QByteArrayData data[124];
    char stringdata0[1837];
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
QT_MOC_LITERAL(9, 148, 10), // "enableBeta"
QT_MOC_LITERAL(10, 159, 11), // "setStepTime"
QT_MOC_LITERAL(11, 171, 5), // "value"
QT_MOC_LITERAL(12, 177, 15), // "setHMDThreshold"
QT_MOC_LITERAL(13, 193, 2), // "xz"
QT_MOC_LITERAL(14, 196, 1), // "y"
QT_MOC_LITERAL(15, 198, 14), // "setUseTrackers"
QT_MOC_LITERAL(16, 213, 3), // "val"
QT_MOC_LITERAL(17, 217, 13), // "setDisableHMD"
QT_MOC_LITERAL(18, 231, 19), // "setTrackerThreshold"
QT_MOC_LITERAL(19, 251, 17), // "setAccuracyButton"
QT_MOC_LITERAL(20, 269, 8), // "buttonId"
QT_MOC_LITERAL(21, 278, 25), // "setAccuracyButtonAsToggle"
QT_MOC_LITERAL(22, 304, 15), // "disableByButton"
QT_MOC_LITERAL(23, 320, 20), // "setHandWalkThreshold"
QT_MOC_LITERAL(24, 341, 13), // "walkThreshold"
QT_MOC_LITERAL(25, 355, 19), // "setHandJogThreshold"
QT_MOC_LITERAL(26, 375, 12), // "jogThreshold"
QT_MOC_LITERAL(27, 388, 19), // "setHandRunThreshold"
QT_MOC_LITERAL(28, 408, 12), // "runThreshold"
QT_MOC_LITERAL(29, 421, 22), // "setScaleTouchWithSwing"
QT_MOC_LITERAL(30, 444, 12), // "setWalkTouch"
QT_MOC_LITERAL(31, 457, 11), // "setJogTouch"
QT_MOC_LITERAL(32, 469, 11), // "setRunTouch"
QT_MOC_LITERAL(33, 481, 21), // "setUseContDirForStraf"
QT_MOC_LITERAL(34, 503, 19), // "setUseContDirForRev"
QT_MOC_LITERAL(35, 523, 15), // "setGameStepType"
QT_MOC_LITERAL(36, 539, 8), // "gameType"
QT_MOC_LITERAL(37, 548, 10), // "setHMDType"
QT_MOC_LITERAL(38, 559, 16), // "setControlSelect"
QT_MOC_LITERAL(39, 576, 7), // "control"
QT_MOC_LITERAL(40, 584, 30), // "setAccuracyButtonControlSelect"
QT_MOC_LITERAL(41, 615, 20), // "setDeviceRenderModel"
QT_MOC_LITERAL(42, 636, 11), // "deviceIndex"
QT_MOC_LITERAL(43, 648, 16), // "renderModelIndex"
QT_MOC_LITERAL(44, 665, 1), // "r"
QT_MOC_LITERAL(45, 667, 1), // "g"
QT_MOC_LITERAL(46, 669, 1), // "b"
QT_MOC_LITERAL(47, 671, 2), // "sx"
QT_MOC_LITERAL(48, 674, 2), // "sy"
QT_MOC_LITERAL(49, 677, 2), // "sz"
QT_MOC_LITERAL(50, 680, 19), // "applyStepPoseDetect"
QT_MOC_LITERAL(51, 700, 26), // "accuracyButtonOnOrDisabled"
QT_MOC_LITERAL(52, 727, 18), // "upAndDownStepCheck"
QT_MOC_LITERAL(53, 746, 17), // "vr::HmdVector3d_t"
QT_MOC_LITERAL(54, 764, 3), // "vel"
QT_MOC_LITERAL(55, 768, 9), // "threshold"
QT_MOC_LITERAL(56, 778, 4), // "roll"
QT_MOC_LITERAL(57, 783, 5), // "pitch"
QT_MOC_LITERAL(58, 789, 8), // "nodCheck"
QT_MOC_LITERAL(59, 798, 19), // "sideToSideStepCheck"
QT_MOC_LITERAL(60, 818, 13), // "isJoggingStep"
QT_MOC_LITERAL(61, 832, 6), // "float*"
QT_MOC_LITERAL(62, 839, 13), // "isRunningStep"
QT_MOC_LITERAL(63, 853, 14), // "getScaledTouch"
QT_MOC_LITERAL(64, 868, 8), // "minTouch"
QT_MOC_LITERAL(65, 877, 8), // "maxTouch"
QT_MOC_LITERAL(66, 886, 6), // "avgVel"
QT_MOC_LITERAL(67, 893, 6), // "maxVel"
QT_MOC_LITERAL(68, 900, 12), // "stopMovement"
QT_MOC_LITERAL(69, 913, 8), // "uint32_t"
QT_MOC_LITERAL(70, 922, 8), // "deviceId"
QT_MOC_LITERAL(71, 931, 17), // "stopClickMovement"
QT_MOC_LITERAL(72, 949, 17), // "applyAxisMovement"
QT_MOC_LITERAL(73, 967, 22), // "vr::VRControllerAxis_t"
QT_MOC_LITERAL(74, 990, 9), // "axisState"
QT_MOC_LITERAL(75, 1000, 18), // "applyClickMovement"
QT_MOC_LITERAL(76, 1019, 17), // "applyGripMovement"
QT_MOC_LITERAL(77, 1037, 9), // "axisEvent"
QT_MOC_LITERAL(78, 1047, 6), // "axisId"
QT_MOC_LITERAL(79, 1054, 1), // "x"
QT_MOC_LITERAL(80, 1056, 11), // "buttonEvent"
QT_MOC_LITERAL(81, 1068, 11), // "buttonState"
QT_MOC_LITERAL(82, 1080, 25), // "updateAccuracyButtonState"
QT_MOC_LITERAL(83, 1106, 15), // "firstController"
QT_MOC_LITERAL(84, 1122, 21), // "addWalkInPlaceProfile"
QT_MOC_LITERAL(85, 1144, 4), // "name"
QT_MOC_LITERAL(86, 1149, 23), // "applyWalkInPlaceProfile"
QT_MOC_LITERAL(87, 1173, 24), // "deleteWalkInPlaceProfile"
QT_MOC_LITERAL(88, 1198, 14), // "getDeviceCount"
QT_MOC_LITERAL(89, 1213, 15), // "getDeviceSerial"
QT_MOC_LITERAL(90, 1229, 11), // "getDeviceId"
QT_MOC_LITERAL(91, 1241, 14), // "getDeviceClass"
QT_MOC_LITERAL(92, 1256, 14), // "getDeviceState"
QT_MOC_LITERAL(93, 1271, 13), // "getDeviceMode"
QT_MOC_LITERAL(94, 1285, 11), // "getStepTime"
QT_MOC_LITERAL(95, 1297, 11), // "getGameType"
QT_MOC_LITERAL(96, 1309, 10), // "getHMDType"
QT_MOC_LITERAL(97, 1320, 16), // "getControlSelect"
QT_MOC_LITERAL(98, 1337, 30), // "getAccuracyButtonControlSelect"
QT_MOC_LITERAL(99, 1368, 17), // "getAccuracyButton"
QT_MOC_LITERAL(100, 1386, 17), // "getHMDXZThreshold"
QT_MOC_LITERAL(101, 1404, 16), // "getHMDYThreshold"
QT_MOC_LITERAL(102, 1421, 14), // "getUseTrackers"
QT_MOC_LITERAL(103, 1436, 13), // "getDisableHMD"
QT_MOC_LITERAL(104, 1450, 21), // "getTrackerXZThreshold"
QT_MOC_LITERAL(105, 1472, 20), // "getTrackerYThreshold"
QT_MOC_LITERAL(106, 1493, 20), // "getHandWalkThreshold"
QT_MOC_LITERAL(107, 1514, 19), // "getHandJogThreshold"
QT_MOC_LITERAL(108, 1534, 19), // "getHandRunThreshold"
QT_MOC_LITERAL(109, 1554, 21), // "getUseContDirForStraf"
QT_MOC_LITERAL(110, 1576, 19), // "getUseContDirForRev"
QT_MOC_LITERAL(111, 1596, 22), // "getScaleTouchWithSwing"
QT_MOC_LITERAL(112, 1619, 12), // "getWalkTouch"
QT_MOC_LITERAL(113, 1632, 11), // "getJogTouch"
QT_MOC_LITERAL(114, 1644, 11), // "getRunTouch"
QT_MOC_LITERAL(115, 1656, 25), // "getAccuracyButtonIsToggle"
QT_MOC_LITERAL(116, 1682, 21), // "getAccuracyButtonFlip"
QT_MOC_LITERAL(117, 1704, 22), // "isStepDetectionEnabled"
QT_MOC_LITERAL(118, 1727, 14), // "isStepDetected"
QT_MOC_LITERAL(119, 1742, 13), // "getGraphPoses"
QT_MOC_LITERAL(120, 1756, 12), // "QList<qreal>"
QT_MOC_LITERAL(121, 1769, 14), // "setupStepGraph"
QT_MOC_LITERAL(122, 1784, 26), // "getWalkInPlaceProfileCount"
QT_MOC_LITERAL(123, 1811, 25) // "getWalkInPlaceProfileName"

    },
    "walkinplace::WalkInPlaceTabController\0"
    "deviceCountChanged\0\0deviceCount\0"
    "deviceInfoChanged\0index\0"
    "walkInPlaceProfilesChanged\0"
    "enableStepDetection\0enable\0enableBeta\0"
    "setStepTime\0value\0setHMDThreshold\0xz\0"
    "y\0setUseTrackers\0val\0setDisableHMD\0"
    "setTrackerThreshold\0setAccuracyButton\0"
    "buttonId\0setAccuracyButtonAsToggle\0"
    "disableByButton\0setHandWalkThreshold\0"
    "walkThreshold\0setHandJogThreshold\0"
    "jogThreshold\0setHandRunThreshold\0"
    "runThreshold\0setScaleTouchWithSwing\0"
    "setWalkTouch\0setJogTouch\0setRunTouch\0"
    "setUseContDirForStraf\0setUseContDirForRev\0"
    "setGameStepType\0gameType\0setHMDType\0"
    "setControlSelect\0control\0"
    "setAccuracyButtonControlSelect\0"
    "setDeviceRenderModel\0deviceIndex\0"
    "renderModelIndex\0r\0g\0b\0sx\0sy\0sz\0"
    "applyStepPoseDetect\0accuracyButtonOnOrDisabled\0"
    "upAndDownStepCheck\0vr::HmdVector3d_t\0"
    "vel\0threshold\0roll\0pitch\0nodCheck\0"
    "sideToSideStepCheck\0isJoggingStep\0"
    "float*\0isRunningStep\0getScaledTouch\0"
    "minTouch\0maxTouch\0avgVel\0maxVel\0"
    "stopMovement\0uint32_t\0deviceId\0"
    "stopClickMovement\0applyAxisMovement\0"
    "vr::VRControllerAxis_t\0axisState\0"
    "applyClickMovement\0applyGripMovement\0"
    "axisEvent\0axisId\0x\0buttonEvent\0"
    "buttonState\0updateAccuracyButtonState\0"
    "firstController\0addWalkInPlaceProfile\0"
    "name\0applyWalkInPlaceProfile\0"
    "deleteWalkInPlaceProfile\0getDeviceCount\0"
    "getDeviceSerial\0getDeviceId\0getDeviceClass\0"
    "getDeviceState\0getDeviceMode\0getStepTime\0"
    "getGameType\0getHMDType\0getControlSelect\0"
    "getAccuracyButtonControlSelect\0"
    "getAccuracyButton\0getHMDXZThreshold\0"
    "getHMDYThreshold\0getUseTrackers\0"
    "getDisableHMD\0getTrackerXZThreshold\0"
    "getTrackerYThreshold\0getHandWalkThreshold\0"
    "getHandJogThreshold\0getHandRunThreshold\0"
    "getUseContDirForStraf\0getUseContDirForRev\0"
    "getScaleTouchWithSwing\0getWalkTouch\0"
    "getJogTouch\0getRunTouch\0"
    "getAccuracyButtonIsToggle\0"
    "getAccuracyButtonFlip\0isStepDetectionEnabled\0"
    "isStepDetected\0getGraphPoses\0QList<qreal>\0"
    "setupStepGraph\0getWalkInPlaceProfileCount\0"
    "getWalkInPlaceProfileName"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_walkinplace__WalkInPlaceTabController[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      81,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  419,    2, 0x06 /* Public */,
       4,    1,  422,    2, 0x06 /* Public */,
       6,    0,  425,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,  426,    2, 0x0a /* Public */,
       9,    1,  429,    2, 0x0a /* Public */,
      10,    1,  432,    2, 0x0a /* Public */,
      12,    2,  435,    2, 0x0a /* Public */,
      15,    1,  440,    2, 0x0a /* Public */,
      17,    1,  443,    2, 0x0a /* Public */,
      18,    2,  446,    2, 0x0a /* Public */,
      19,    1,  451,    2, 0x0a /* Public */,
      21,    1,  454,    2, 0x0a /* Public */,
      22,    1,  457,    2, 0x0a /* Public */,
      23,    1,  460,    2, 0x0a /* Public */,
      25,    1,  463,    2, 0x0a /* Public */,
      27,    1,  466,    2, 0x0a /* Public */,
      29,    1,  469,    2, 0x0a /* Public */,
      30,    1,  472,    2, 0x0a /* Public */,
      31,    1,  475,    2, 0x0a /* Public */,
      32,    1,  478,    2, 0x0a /* Public */,
      33,    1,  481,    2, 0x0a /* Public */,
      34,    1,  484,    2, 0x0a /* Public */,
      35,    1,  487,    2, 0x0a /* Public */,
      37,    1,  490,    2, 0x0a /* Public */,
      38,    1,  493,    2, 0x0a /* Public */,
      40,    1,  496,    2, 0x0a /* Public */,
      41,    8,  499,    2, 0x0a /* Public */,
      50,    0,  516,    2, 0x0a /* Public */,
      51,    0,  517,    2, 0x0a /* Public */,
      52,    4,  518,    2, 0x0a /* Public */,
      58,    2,  527,    2, 0x0a /* Public */,
      59,    2,  532,    2, 0x0a /* Public */,
      60,    1,  537,    2, 0x0a /* Public */,
      62,    1,  540,    2, 0x0a /* Public */,
      63,    4,  543,    2, 0x0a /* Public */,
      68,    1,  552,    2, 0x0a /* Public */,
      71,    1,  555,    2, 0x0a /* Public */,
      72,    2,  558,    2, 0x0a /* Public */,
      75,    1,  563,    2, 0x0a /* Public */,
      76,    1,  566,    2, 0x0a /* Public */,
      77,    4,  569,    2, 0x0a /* Public */,
      80,    3,  578,    2, 0x0a /* Public */,
      82,    2,  585,    2, 0x0a /* Public */,
      84,    1,  590,    2, 0x0a /* Public */,
      86,    1,  593,    2, 0x0a /* Public */,
      87,    1,  596,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
      88,    0,  599,    2, 0x02 /* Public */,
      89,    1,  600,    2, 0x02 /* Public */,
      90,    1,  603,    2, 0x02 /* Public */,
      91,    1,  606,    2, 0x02 /* Public */,
      92,    1,  609,    2, 0x02 /* Public */,
      93,    1,  612,    2, 0x02 /* Public */,
      94,    0,  615,    2, 0x02 /* Public */,
      95,    0,  616,    2, 0x02 /* Public */,
      96,    0,  617,    2, 0x02 /* Public */,
      97,    0,  618,    2, 0x02 /* Public */,
      98,    0,  619,    2, 0x02 /* Public */,
      99,    0,  620,    2, 0x02 /* Public */,
     100,    0,  621,    2, 0x02 /* Public */,
     101,    0,  622,    2, 0x02 /* Public */,
     102,    0,  623,    2, 0x02 /* Public */,
     103,    0,  624,    2, 0x02 /* Public */,
     104,    0,  625,    2, 0x02 /* Public */,
     105,    0,  626,    2, 0x02 /* Public */,
     106,    0,  627,    2, 0x02 /* Public */,
     107,    0,  628,    2, 0x02 /* Public */,
     108,    0,  629,    2, 0x02 /* Public */,
     109,    0,  630,    2, 0x02 /* Public */,
     110,    0,  631,    2, 0x02 /* Public */,
     111,    0,  632,    2, 0x02 /* Public */,
     112,    0,  633,    2, 0x02 /* Public */,
     113,    0,  634,    2, 0x02 /* Public */,
     114,    0,  635,    2, 0x02 /* Public */,
     115,    0,  636,    2, 0x02 /* Public */,
     116,    0,  637,    2, 0x02 /* Public */,
     117,    0,  638,    2, 0x02 /* Public */,
     118,    0,  639,    2, 0x02 /* Public */,
     119,    0,  640,    2, 0x02 /* Public */,
     121,    0,  641,    2, 0x02 /* Public */,
     122,    0,  642,    2, 0x02 /* Public */,
     123,    1,  643,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::UInt,    3,
    QMetaType::Void, QMetaType::UInt,    5,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Double,   11,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   13,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Float, QMetaType::Float,   13,   14,
    QMetaType::Void, QMetaType::Int,   20,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Float,   24,
    QMetaType::Void, QMetaType::Float,   26,
    QMetaType::Void, QMetaType::Float,   28,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Float,   11,
    QMetaType::Void, QMetaType::Float,   11,
    QMetaType::Void, QMetaType::Float,   11,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::Int,   39,
    QMetaType::Void, QMetaType::Int,   39,
    QMetaType::Void, QMetaType::UInt, QMetaType::UInt, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   42,   43,   44,   45,   46,   47,   48,   49,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool, 0x80000000 | 53, 0x80000000 | 53, QMetaType::Double, QMetaType::Double,   54,   55,   56,   57,
    QMetaType::Bool, 0x80000000 | 53, 0x80000000 | 53,   54,   55,
    QMetaType::Bool, 0x80000000 | 53, 0x80000000 | 53,   54,   55,
    QMetaType::Bool, 0x80000000 | 61,   54,
    QMetaType::Bool, 0x80000000 | 61,   54,
    QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float, QMetaType::Float,   64,   65,   66,   67,
    QMetaType::Void, 0x80000000 | 69,   70,
    QMetaType::Void, 0x80000000 | 69,   70,
    QMetaType::Void, 0x80000000 | 69, 0x80000000 | 73,   70,   74,
    QMetaType::Void, 0x80000000 | 69,   70,
    QMetaType::Void, 0x80000000 | 69,   70,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Float, QMetaType::Float,   70,   78,   79,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   70,   20,   81,
    QMetaType::Void, 0x80000000 | 69, QMetaType::Bool,   70,   83,
    QMetaType::Void, QMetaType::QString,   85,
    QMetaType::Void, QMetaType::UInt,    5,
    QMetaType::Void, QMetaType::UInt,    5,

 // methods: parameters
    QMetaType::UInt,
    QMetaType::QString, QMetaType::UInt,    5,
    QMetaType::UInt, QMetaType::UInt,    5,
    QMetaType::Int, QMetaType::UInt,    5,
    QMetaType::Int, QMetaType::UInt,    5,
    QMetaType::Int, QMetaType::UInt,    5,
    QMetaType::Double,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Float,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Bool,
    0x80000000 | 120,
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
        case 4: _t->enableBeta((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->setStepTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->setHMDThreshold((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 7: _t->setUseTrackers((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->setDisableHMD((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setTrackerThreshold((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2]))); break;
        case 10: _t->setAccuracyButton((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->setAccuracyButtonAsToggle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->disableByButton((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->setHandWalkThreshold((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 14: _t->setHandJogThreshold((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 15: _t->setHandRunThreshold((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 16: _t->setScaleTouchWithSwing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->setWalkTouch((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 18: _t->setJogTouch((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 19: _t->setRunTouch((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 20: _t->setUseContDirForStraf((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 21: _t->setUseContDirForRev((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->setGameStepType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 23: _t->setHMDType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->setControlSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->setAccuracyButtonControlSelect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 26: _t->setDeviceRenderModel((*reinterpret_cast< uint(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6])),(*reinterpret_cast< float(*)>(_a[7])),(*reinterpret_cast< float(*)>(_a[8]))); break;
        case 27: _t->applyStepPoseDetect(); break;
        case 28: { bool _r = _t->accuracyButtonOnOrDisabled();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 29: { bool _r = _t->upAndDownStepCheck((*reinterpret_cast< vr::HmdVector3d_t(*)>(_a[1])),(*reinterpret_cast< vr::HmdVector3d_t(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 30: { bool _r = _t->nodCheck((*reinterpret_cast< vr::HmdVector3d_t(*)>(_a[1])),(*reinterpret_cast< vr::HmdVector3d_t(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 31: { bool _r = _t->sideToSideStepCheck((*reinterpret_cast< vr::HmdVector3d_t(*)>(_a[1])),(*reinterpret_cast< vr::HmdVector3d_t(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 32: { bool _r = _t->isJoggingStep((*reinterpret_cast< float*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 33: { bool _r = _t->isRunningStep((*reinterpret_cast< float*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 34: { float _r = _t->getScaledTouch((*reinterpret_cast< float(*)>(_a[1])),(*reinterpret_cast< float(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 35: _t->stopMovement((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 36: _t->stopClickMovement((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 37: _t->applyAxisMovement((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< vr::VRControllerAxis_t(*)>(_a[2]))); break;
        case 38: _t->applyClickMovement((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 39: _t->applyGripMovement((*reinterpret_cast< uint32_t(*)>(_a[1]))); break;
        case 40: _t->axisEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4]))); break;
        case 41: _t->buttonEvent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 42: _t->updateAccuracyButtonState((*reinterpret_cast< uint32_t(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 43: _t->addWalkInPlaceProfile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 44: _t->applyWalkInPlaceProfile((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 45: _t->deleteWalkInPlaceProfile((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 46: { uint _r = _t->getDeviceCount();
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 47: { QString _r = _t->getDeviceSerial((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        case 48: { uint _r = _t->getDeviceId((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 49: { int _r = _t->getDeviceClass((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 50: { int _r = _t->getDeviceState((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 51: { int _r = _t->getDeviceMode((*reinterpret_cast< uint(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 52: { double _r = _t->getStepTime();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 53: { int _r = _t->getGameType();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 54: { int _r = _t->getHMDType();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 55: { int _r = _t->getControlSelect();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 56: { int _r = _t->getAccuracyButtonControlSelect();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 57: { int _r = _t->getAccuracyButton();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 58: { float _r = _t->getHMDXZThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 59: { float _r = _t->getHMDYThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 60: { bool _r = _t->getUseTrackers();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 61: { bool _r = _t->getDisableHMD();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 62: { float _r = _t->getTrackerXZThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 63: { float _r = _t->getTrackerYThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 64: { float _r = _t->getHandWalkThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 65: { float _r = _t->getHandJogThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 66: { float _r = _t->getHandRunThreshold();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 67: { bool _r = _t->getUseContDirForStraf();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 68: { bool _r = _t->getUseContDirForRev();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 69: { bool _r = _t->getScaleTouchWithSwing();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 70: { float _r = _t->getWalkTouch();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 71: { float _r = _t->getJogTouch();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 72: { float _r = _t->getRunTouch();
            if (_a[0]) *reinterpret_cast< float*>(_a[0]) = std::move(_r); }  break;
        case 73: { bool _r = _t->getAccuracyButtonIsToggle();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 74: { bool _r = _t->getAccuracyButtonFlip();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 75: { bool _r = _t->isStepDetectionEnabled();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 76: { bool _r = _t->isStepDetected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 77: { QList<qreal> _r = _t->getGraphPoses();
            if (_a[0]) *reinterpret_cast< QList<qreal>*>(_a[0]) = std::move(_r); }  break;
        case 78: _t->setupStepGraph(); break;
        case 79: { uint _r = _t->getWalkInPlaceProfileCount();
            if (_a[0]) *reinterpret_cast< uint*>(_a[0]) = std::move(_r); }  break;
        case 80: { QString _r = _t->getWalkInPlaceProfileName((*reinterpret_cast< uint(*)>(_a[1])));
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
        if (_id < 81)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 81;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 81)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 81;
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
