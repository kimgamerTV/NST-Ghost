/****************************************************************************
** Meta object code from reading C++ file 'translationservicemanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/managers/translationservicemanager.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'translationservicemanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN25TranslationServiceManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto TranslationServiceManager::qt_create_metaobjectdata<qt_meta_tag_ZN25TranslationServiceManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TranslationServiceManager",
        "translationFinished",
        "",
        "qtlingo::TranslationResult",
        "result",
        "errorOccurred",
        "message",
        "progressUpdated",
        "current",
        "total",
        "processNextTranslation",
        "onTranslationDone",
        "onBatchTranslationDone",
        "QList<qtlingo::TranslationResult>",
        "results",
        "onTranslationError"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'translationFinished'
        QtMocHelpers::SignalData<void(const qtlingo::TranslationResult &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'progressUpdated'
        QtMocHelpers::SignalData<void(int, int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::Int, 9 },
        }}),
        // Slot 'processNextTranslation'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTranslationDone'
        QtMocHelpers::SlotData<void(const qtlingo::TranslationResult &)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'onBatchTranslationDone'
        QtMocHelpers::SlotData<void(const QList<qtlingo::TranslationResult> &)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 13, 14 },
        }}),
        // Slot 'onTranslationError'
        QtMocHelpers::SlotData<void(const QString &)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TranslationServiceManager, qt_meta_tag_ZN25TranslationServiceManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TranslationServiceManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25TranslationServiceManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25TranslationServiceManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN25TranslationServiceManagerE_t>.metaTypes,
    nullptr
} };

void TranslationServiceManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TranslationServiceManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->translationFinished((*reinterpret_cast<std::add_pointer_t<qtlingo::TranslationResult>>(_a[1]))); break;
        case 1: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->progressUpdated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 3: _t->processNextTranslation(); break;
        case 4: _t->onTranslationDone((*reinterpret_cast<std::add_pointer_t<qtlingo::TranslationResult>>(_a[1]))); break;
        case 5: _t->onBatchTranslationDone((*reinterpret_cast<std::add_pointer_t<QList<qtlingo::TranslationResult>>>(_a[1]))); break;
        case 6: _t->onTranslationError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TranslationServiceManager::*)(const qtlingo::TranslationResult & )>(_a, &TranslationServiceManager::translationFinished, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (TranslationServiceManager::*)(const QString & )>(_a, &TranslationServiceManager::errorOccurred, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (TranslationServiceManager::*)(int , int )>(_a, &TranslationServiceManager::progressUpdated, 2))
            return;
    }
}

const QMetaObject *TranslationServiceManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TranslationServiceManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN25TranslationServiceManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TranslationServiceManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void TranslationServiceManager::translationFinished(const qtlingo::TranslationResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void TranslationServiceManager::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void TranslationServiceManager::progressUpdated(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}
QT_WARNING_POP
