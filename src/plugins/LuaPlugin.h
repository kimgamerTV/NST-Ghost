#pragma once
#include <QObject>
#include <QVariant>
#include <lua.hpp>
#include <memory>

class LuaPlugin : public QObject {
    Q_OBJECT
public:
    explicit LuaPlugin(QObject* parent = nullptr);
    ~LuaPlugin();

    bool loadScript(const QString& scriptPath);
    QVariant callFunction(const QString& funcName, const QVariantList& args = {});
    bool hasFunction(const QString& funcName);
    void registerFunction(const QString& name, lua_CFunction func);

signals:
    void scriptError(const QString& error);

private:
    std::unique_ptr<lua_State, decltype(&lua_close)> m_lua;
};
