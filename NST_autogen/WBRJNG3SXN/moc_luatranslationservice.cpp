/****************************************************************************
** Meta object code from reading C++ file 'luatranslationservice.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/plugins/LuaTranslationPlugin/luatranslationservice.h"
#include <QtNetwork/QSslError>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'luatranslationservice.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9LuaWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto LuaWorker::qt_create_metaobjectdata<qt_meta_tag_ZN9LuaWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LuaWorker",
        "translationFinished",
        "",
        "qtlingo::TranslationResult",
        "result",
        "batchTranslationFinished",
        "QList<qtlingo::TranslationResult>",
        "results",
        "errorOccurred",
        "message",
        "logMessage",
        "processTranslation",
        "sourceText",
        "processBatchTranslation",
        "sourceTexts"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'translationFinished'
        QtMocHelpers::SignalData<void(const qtlingo::TranslationResult &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'batchTranslationFinished'
        QtMocHelpers::SignalData<void(const QList<qtlingo::TranslationResult> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 7 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Signal 'logMessage'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'processTranslation'
        QtMocHelpers::SlotData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Slot 'processBatchTranslation'
        QtMocHelpers::SlotData<void(const QStringList &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 14 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LuaWorker, qt_meta_tag_ZN9LuaWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LuaWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9LuaWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9LuaWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9LuaWorkerE_t>.metaTypes,
    nullptr
} };

void LuaWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LuaWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->translationFinished((*reinterpret_cast<std::add_pointer_t<qtlingo::TranslationResult>>(_a[1]))); break;
        case 1: _t->batchTranslationFinished((*reinterpret_cast<std::add_pointer_t<QList<qtlingo::TranslationResult>>>(_a[1]))); break;
        case 2: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->logMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->processTranslation((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->processBatchTranslation((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (LuaWorker::*)(const qtlingo::TranslationResult & )>(_a, &LuaWorker::translationFinished, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (LuaWorker::*)(const QList<qtlingo::TranslationResult> & )>(_a, &LuaWorker::batchTranslationFinished, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (LuaWorker::*)(const QString & )>(_a, &LuaWorker::errorOccurred, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (LuaWorker::*)(const QString & )>(_a, &LuaWorker::logMessage, 3))
            return;
    }
}

const QMetaObject *LuaWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LuaWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9LuaWorkerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LuaWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void LuaWorker::translationFinished(const qtlingo::TranslationResult & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void LuaWorker::batchTranslationFinished(const QList<qtlingo::TranslationResult> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void LuaWorker::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void LuaWorker::logMessage(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN21LuaTranslationServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto LuaTranslationService::qt_create_metaobjectdata<qt_meta_tag_ZN21LuaTranslationServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LuaTranslationService",
        "startTranslation",
        "",
        "sourceText",
        "startBatchTranslation",
        "sourceTexts",
        "logMessage",
        "message"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'startTranslation'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'startBatchTranslation'
        QtMocHelpers::SignalData<void(const QStringList &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 5 },
        }}),
        // Signal 'logMessage'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LuaTranslationService, qt_meta_tag_ZN21LuaTranslationServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LuaTranslationService::staticMetaObject = { {
    QMetaObject::SuperData::link<qtlingo::ITranslationService::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21LuaTranslationServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21LuaTranslationServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN21LuaTranslationServiceE_t>.metaTypes,
    nullptr
} };

void LuaTranslationService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LuaTranslationService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->startTranslation((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->startBatchTranslation((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 2: _t->logMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (LuaTranslationService::*)(const QString & )>(_a, &LuaTranslationService::startTranslation, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (LuaTranslationService::*)(const QStringList & )>(_a, &LuaTranslationService::startBatchTranslation, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (LuaTranslationService::*)(const QString & )>(_a, &LuaTranslationService::logMessage, 2))
            return;
    }
}

const QMetaObject *LuaTranslationService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LuaTranslationService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21LuaTranslationServiceE_t>.strings))
        return static_cast<void*>(this);
    return qtlingo::ITranslationService::qt_metacast(_clname);
}

int LuaTranslationService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = qtlingo::ITranslationService::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void LuaTranslationService::startTranslation(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void LuaTranslationService::startBatchTranslation(const QStringList & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void LuaTranslationService::logMessage(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}
QT_WARNING_POP
