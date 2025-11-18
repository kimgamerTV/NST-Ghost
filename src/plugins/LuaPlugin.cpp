#include "LuaPlugin.h"
#include <QFile>
#include <QDebug>

LuaPlugin::LuaPlugin(QObject* parent) 
    : QObject(parent), m_lua(luaL_newstate(), lua_close) {
    luaL_openlibs(m_lua.get());
}

LuaPlugin::~LuaPlugin() = default;

bool LuaPlugin::loadScript(const QString& scriptPath) {
    QFile file(scriptPath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit scriptError("Cannot open: " + scriptPath);
        return false;
    }

    QByteArray script = file.readAll();
    if (luaL_dostring(m_lua.get(), script.constData()) != LUA_OK) {
        emit scriptError(lua_tostring(m_lua.get(), -1));
        lua_pop(m_lua.get(), 1);
        return false;
    }
    return true;
}

QVariant LuaPlugin::callFunction(const QString& funcName, const QVariantList& args) {
    lua_getglobal(m_lua.get(), funcName.toUtf8().constData());
    
    for (const auto& arg : args) {
        if (arg.typeId() == QMetaType::QString) {
            lua_pushstring(m_lua.get(), arg.toString().toUtf8().constData());
        } else if (arg.typeId() == QMetaType::Int) {
            lua_pushinteger(m_lua.get(), arg.toInt());
        } else if (arg.typeId() == QMetaType::Double) {
            lua_pushnumber(m_lua.get(), arg.toDouble());
        }
    }

    if (lua_pcall(m_lua.get(), args.size(), 1, 0) != LUA_OK) {
        emit scriptError(lua_tostring(m_lua.get(), -1));
        lua_pop(m_lua.get(), 1);
        return {};
    }

    QVariant result;
    if (lua_isstring(m_lua.get(), -1)) {
        result = QString::fromUtf8(lua_tostring(m_lua.get(), -1));
    } else if (lua_isnumber(m_lua.get(), -1)) {
        result = lua_tonumber(m_lua.get(), -1);
    }
    lua_pop(m_lua.get(), 1);
    return result;
}

void LuaPlugin::registerFunction(const QString& name, lua_CFunction func) {
    lua_register(m_lua.get(), name.toUtf8().constData(), func);
}
