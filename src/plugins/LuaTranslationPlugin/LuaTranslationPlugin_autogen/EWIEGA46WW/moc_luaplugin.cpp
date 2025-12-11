/****************************************************************************
** Meta object code from reading C++ file 'luaplugin.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../luaplugin.h"
#include <QtCore/qmetatype.h>
#include <QtCore/qplugin.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'luaplugin.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN20LuaTranslationPluginE_t {};
} // unnamed namespace

template <> constexpr inline auto LuaTranslationPlugin::qt_create_metaobjectdata<qt_meta_tag_ZN20LuaTranslationPluginE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LuaTranslationPlugin"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LuaTranslationPlugin, qt_meta_tag_ZN20LuaTranslationPluginE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LuaTranslationPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20LuaTranslationPluginE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20LuaTranslationPluginE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20LuaTranslationPluginE_t>.metaTypes,
    nullptr
} };

void LuaTranslationPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LuaTranslationPlugin *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *LuaTranslationPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LuaTranslationPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20LuaTranslationPluginE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "qtlingo::TranslationPluginInterface"))
        return static_cast< qtlingo::TranslationPluginInterface*>(this);
    if (!strcmp(_clname, "org.qtlingo.TranslationPluginInterface"))
        return static_cast< qtlingo::TranslationPluginInterface*>(this);
    return QObject::qt_metacast(_clname);
}

int LuaTranslationPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}

#ifdef QT_MOC_EXPORT_PLUGIN_V2
static constexpr unsigned char qt_pluginMetaDataV2_LuaTranslationPlugin[] = {
    0xbf, 
    // "IID"
    0x02,  0x78,  0x26,  'o',  'r',  'g',  '.',  'q', 
    't',  'l',  'i',  'n',  'g',  'o',  '.',  'T', 
    'r',  'a',  'n',  's',  'l',  'a',  't',  'i', 
    'o',  'n',  'P',  'l',  'u',  'g',  'i',  'n', 
    'I',  'n',  't',  'e',  'r',  'f',  'a',  'c', 
    'e', 
    // "className"
    0x03,  0x74,  'L',  'u',  'a',  'T',  'r',  'a', 
    'n',  's',  'l',  'a',  't',  'i',  'o',  'n', 
    'P',  'l',  'u',  'g',  'i',  'n', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN_V2(LuaTranslationPlugin, LuaTranslationPlugin, qt_pluginMetaDataV2_LuaTranslationPlugin)
#else
QT_PLUGIN_METADATA_SECTION
Q_CONSTINIT static constexpr unsigned char qt_pluginMetaData_LuaTranslationPlugin[] = {
    'Q', 'T', 'M', 'E', 'T', 'A', 'D', 'A', 'T', 'A', ' ', '!',
    // metadata version, Qt version, architectural requirements
    0, QT_VERSION_MAJOR, QT_VERSION_MINOR, qPluginArchRequirements(),
    0xbf, 
    // "IID"
    0x02,  0x78,  0x26,  'o',  'r',  'g',  '.',  'q', 
    't',  'l',  'i',  'n',  'g',  'o',  '.',  'T', 
    'r',  'a',  'n',  's',  'l',  'a',  't',  'i', 
    'o',  'n',  'P',  'l',  'u',  'g',  'i',  'n', 
    'I',  'n',  't',  'e',  'r',  'f',  'a',  'c', 
    'e', 
    // "className"
    0x03,  0x74,  'L',  'u',  'a',  'T',  'r',  'a', 
    'n',  's',  'l',  'a',  't',  'i',  'o',  'n', 
    'P',  'l',  'u',  'g',  'i',  'n', 
    0xff, 
};
QT_MOC_EXPORT_PLUGIN(LuaTranslationPlugin, LuaTranslationPlugin)
#endif  // QT_MOC_EXPORT_PLUGIN_V2

QT_WARNING_POP
