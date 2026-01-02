/****************************************************************************
** Meta object code from reading C++ file 'filetranslationwidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ui/filetranslationwidget.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'filetranslationwidget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN21FileTranslationWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto FileTranslationWidget::qt_create_metaobjectdata<qt_meta_tag_ZN21FileTranslationWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FileTranslationWidget",
        "projectLoaded",
        "",
        "projectPath",
        "openSearchDialog",
        "onSelectAllRequested",
        "onLoadingFinished",
        "onProjectProcessingFinished",
        "onSearchResultSelected",
        "fileName",
        "row",
        "onBGADataError",
        "message",
        "onFontsLoaded",
        "QJsonArray",
        "fonts",
        "onSearchRequested",
        "query",
        "onTranslationFinished",
        "qtlingo::TranslationResult",
        "result",
        "onTranslationServiceError",
        "onTranslationTableViewCustomContextMenuRequested",
        "QPoint",
        "pos",
        "onTranslateSelectedTextWithService",
        "onTranslateAllSelectedText",
        "onTranslateSelectedFiles",
        "onTranslationDataChanged",
        "QModelIndex",
        "topLeft",
        "bottomRight",
        "onFileListCustomContextMenuRequested",
        "onMarkAsIgnored",
        "onUnmarkAsIgnored",
        "onAILearnRequested",
        "onAIUnlearnRequested",
        "processIncomingResults"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'projectLoaded'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Slot 'openSearchDialog'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSelectAllRequested'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onLoadingFinished'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onProjectProcessingFinished'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSearchResultSelected'
        QtMocHelpers::SlotData<void(const QString &, int)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 }, { QMetaType::Int, 10 },
        }}),
        // Slot 'onBGADataError'
        QtMocHelpers::SlotData<void(const QString &)>(11, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Slot 'onFontsLoaded'
        QtMocHelpers::SlotData<void(const QJsonArray &)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Slot 'onSearchRequested'
        QtMocHelpers::SlotData<void(const QString &)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Slot 'onTranslationFinished'
        QtMocHelpers::SlotData<void(const qtlingo::TranslationResult &)>(18, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 19, 20 },
        }}),
        // Slot 'onTranslationServiceError'
        QtMocHelpers::SlotData<void(const QString &)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Slot 'onTranslationTableViewCustomContextMenuRequested'
        QtMocHelpers::SlotData<void(const QPoint &)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 23, 24 },
        }}),
        // Slot 'onTranslateSelectedTextWithService'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTranslateAllSelectedText'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTranslateSelectedFiles'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onTranslationDataChanged'
        QtMocHelpers::SlotData<void(const QModelIndex &, const QModelIndex &)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 29, 30 }, { 0x80000000 | 29, 31 },
        }}),
        // Slot 'onFileListCustomContextMenuRequested'
        QtMocHelpers::SlotData<void(const QPoint &)>(32, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 23, 24 },
        }}),
        // Slot 'onMarkAsIgnored'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUnmarkAsIgnored'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAILearnRequested'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAIUnlearnRequested'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'processIncomingResults'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FileTranslationWidget, qt_meta_tag_ZN21FileTranslationWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FileTranslationWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21FileTranslationWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21FileTranslationWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN21FileTranslationWidgetE_t>.metaTypes,
    nullptr
} };

void FileTranslationWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FileTranslationWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->projectLoaded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->openSearchDialog(); break;
        case 2: _t->onSelectAllRequested(); break;
        case 3: _t->onLoadingFinished(); break;
        case 4: _t->onProjectProcessingFinished(); break;
        case 5: _t->onSearchResultSelected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 6: _t->onBGADataError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onFontsLoaded((*reinterpret_cast<std::add_pointer_t<QJsonArray>>(_a[1]))); break;
        case 8: _t->onSearchRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->onTranslationFinished((*reinterpret_cast<std::add_pointer_t<qtlingo::TranslationResult>>(_a[1]))); break;
        case 10: _t->onTranslationServiceError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onTranslationTableViewCustomContextMenuRequested((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 12: _t->onTranslateSelectedTextWithService(); break;
        case 13: _t->onTranslateAllSelectedText(); break;
        case 14: _t->onTranslateSelectedFiles(); break;
        case 15: _t->onTranslationDataChanged((*reinterpret_cast<std::add_pointer_t<QModelIndex>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QModelIndex>>(_a[2]))); break;
        case 16: _t->onFileListCustomContextMenuRequested((*reinterpret_cast<std::add_pointer_t<QPoint>>(_a[1]))); break;
        case 17: _t->onMarkAsIgnored(); break;
        case 18: _t->onUnmarkAsIgnored(); break;
        case 19: _t->onAILearnRequested(); break;
        case 20: _t->onAIUnlearnRequested(); break;
        case 21: _t->processIncomingResults(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FileTranslationWidget::*)(const QString & )>(_a, &FileTranslationWidget::projectLoaded, 0))
            return;
    }
}

const QMetaObject *FileTranslationWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FileTranslationWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN21FileTranslationWidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int FileTranslationWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void FileTranslationWidget::projectLoaded(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
QT_WARNING_POP
