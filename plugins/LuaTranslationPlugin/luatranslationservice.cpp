#include "luatranslationservice.h"
#include <QFileInfo>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>

// --- LuaWorker Implementation ---

LuaWorker::LuaWorker(const QString &scriptPath)
    : m_scriptPath(scriptPath)
{
    initLua();
}

LuaWorker::~LuaWorker()
{
    if (L) {
        lua_close(L);
    }
}

bool LuaWorker::initLua()
{
    L = luaL_newstate();
    luaL_openlibs(L);
    // Set script name global
    QFileInfo fi(m_scriptPath);
    lua_pushstring(L, fi.fileName().toUtf8().constData());
    lua_setglobal(L, "__script_name");

    // Register HTTP function
    lua_register(L, "nst_http_request", lua_http_request);
    
    // Register JSON functions
    lua_register(L, "nst_json_encode", [](lua_State* L) -> int {
        // Simple implementation: expects a table, converts to JSON string
        // Note: This is a basic implementation. For full Lua->JSON support, 
        // we'd need a recursive converter. For now, let's assume simple string/number tables.
        // Actually, let's use a proper recursive helper if possible, or just QJsonDocument::fromJson for decoding
        // and QJsonDocument::toJson for encoding.
        
        // For this task, I'll implement a basic wrapper. 
        // Converting Lua table to QJsonObject/QJsonArray is non-trivial without a helper.
        // Let's rely on the user passing a string for body for now in the example?
        // No, the user wants to see if there are problems. JSON IS the problem.
        // I will implement a "good enough" encoder for the Groq payload (nested tables).
        
        // ...actually, writing a full Lua<->QJson converter in a lambda is too much.
        // I'll add static helper methods to LuaWorker.
        return LuaWorker::lua_json_encode(L);
    });

    lua_register(L, "nst_json_decode", [](lua_State* L) -> int {
        return LuaWorker::lua_json_decode(L);
    });

    // Register setting function
    lua_register(L, "nst_get_setting", [](lua_State* L) -> int {
        return LuaWorker::lua_get_setting(L);
    });

    // Register logging function
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, LuaWorker::lua_log, 1);
    lua_setglobal(L, "nst_log");

    if (luaL_dofile(L, m_scriptPath.toStdString().c_str()) != LUA_OK) {
        QString error = lua_tostring(L, -1);
        qCritical() << "Failed to load Lua script:" << m_scriptPath << error;
        emit errorOccurred(QString("Failed to load script: %1").arg(error));
        lua_close(L);
        L = nullptr;
        return false;
    }
    return true;
}

int LuaWorker::lua_log(lua_State *L)
{
    // Get 'this' from upvalue
    LuaWorker* worker = static_cast<LuaWorker*>(lua_touserdata(L, lua_upvalueindex(1)));
    const char* msg = lua_tostring(L, 1);
    qDebug() << "[Lua]" << msg;
    if (worker) {
        emit worker->logMessage(QString::fromUtf8(msg));
    }
    return 0;
}

int LuaWorker::lua_http_request(lua_State *L)
{
    // Arguments: url, method, headers (table), body
    const char* urlStr = luaL_checkstring(L, 1);
    const char* method = luaL_optstring(L, 2, "GET");
    
    QNetworkRequest request(QUrl(QString::fromUtf8(urlStr)));
    
    // Process headers
    if (lua_istable(L, 3)) {
        lua_pushnil(L);
        while (lua_next(L, 3) != 0) {
            const char* key = lua_tostring(L, -2);
            const char* value = lua_tostring(L, -1);
            request.setRawHeader(QByteArray(key), QByteArray(value));
            lua_pop(L, 1);
        }
    }
    
    QByteArray body;
    if (lua_isstring(L, 4)) {
        body = QByteArray(lua_tostring(L, 4));
    }

    // Perform request synchronously (blocking this worker thread)
    QNetworkAccessManager manager;
    QNetworkReply *reply = nullptr;
    
    if (QString::compare(method, "POST", Qt::CaseInsensitive) == 0) {
        reply = manager.post(request, body);
    } else {
        reply = manager.get(request);
    }

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // Return result: body, status_code
    QByteArray responseBody = reply->readAll();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    lua_pushlstring(L, responseBody.constData(), responseBody.size());
    lua_pushinteger(L, statusCode);
    
    reply->deleteLater();
    return 2;
}

// --- JSON Helpers ---

int LuaWorker::lua_json_encode(lua_State *L)
{
    if (lua_gettop(L) < 1) return 0;
    QJsonValue val = lua_to_json(L, 1);
    QJsonDocument doc;
    if (val.isObject()) doc.setObject(val.toObject());
    else if (val.isArray()) doc.setArray(val.toArray());
    else {
        // Wrap primitive in array or just to string? 
        // QJsonDocument can't hold primitive directly.
        // For Groq, we usually send an object.
        lua_pushstring(L, ""); 
        return 1;
    }
    
    lua_pushstring(L, doc.toJson(QJsonDocument::Compact).constData());
    return 1;
}

int LuaWorker::lua_json_decode(lua_State *L)
{
    const char* jsonStr = luaL_checkstring(L, 1);
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray(jsonStr));
    
    if (doc.isArray()) {
        json_to_lua(L, doc.array());
    } else if (doc.isObject()) {
        json_to_lua(L, doc.object());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

int LuaWorker::lua_get_setting(lua_State *L)
{
    // Arguments: key
    const char* key = luaL_checkstring(L, 1);
    
    // Get script filename to use as group
    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "getinfo");
    lua_pushinteger(L, 1);
    lua_pushstring(L, "S");
    lua_call(L, 2, 1);
    lua_getfield(L, -1, "short_src");
    QString scriptPath = QString::fromUtf8(lua_tostring(L, -1));
    lua_pop(L, 3); // Pop short_src, info table, debug
    
    QFileInfo fi(scriptPath);
    QString scriptName = fi.fileName();
    
    // If scriptPath is not available (e.g. running from string), fallback
    // But we always run from file in this plugin.
    // Actually, simpler: we know m_scriptPath in LuaWorker instance, 
    // but this is a static method.
    // We can pass the worker instance as upvalue or just use the global QSettings.
    // Since it's static, we can't access m_scriptPath directly.
    // BUT, we can get the script path from Lua debug info as above, OR
    // we can capture 'this' in the lambda if we make it non-static or use a closure.
    
    // Let's use the lambda capture approach in initLua instead of static method for this one,
    // so we can access m_scriptPath.
    // Wait, I declared it static in header. Let's change it to non-static or use the debug info trick.
    // The debug info trick is reliable enough for `dofile`.
    
    // Actually, let's just use the lambda in initLua to capture 'this'.
    // But I already wrote the static declaration.
    // Let's stick to static and use the debug info or just pass the script name as a global when initializing.
    
    // Better: Set a global variable `__script_name` in initLua.
    lua_getglobal(L, "__script_name");
    QString currentScriptName = QString::fromUtf8(lua_tostring(L, -1));
    lua_pop(L, 1);
    
    if (currentScriptName.isEmpty()) {
         // Fallback
         currentScriptName = "UnknownScript";
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "NST", "PluginSettings");
    settings.beginGroup("Plugins");
    settings.beginGroup(currentScriptName);
    settings.beginGroup("Settings");
    
    QVariant val = settings.value(key);
    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
    
    if (val.isValid()) {
        lua_pushstring(L, val.toString().toUtf8().constData());
    } else {
        lua_pushnil(L);
    }
    return 1;
}

QJsonValue LuaWorker::lua_to_json(lua_State *L, int index)
{
    int type = lua_type(L, index);
    switch (type) {
        case LUA_TBOOLEAN: return QJsonValue((bool)lua_toboolean(L, index));
        case LUA_TNUMBER: return QJsonValue(lua_tonumber(L, index));
        case LUA_TSTRING: return QJsonValue(lua_tostring(L, index));
        case LUA_TTABLE: {
            // Check if array or object
            // Simple heuristic: if key 1 exists, assume array, else object
            lua_pushinteger(L, 1);
            lua_gettable(L, index);
            bool isArray = !lua_isnil(L, -1);
            lua_pop(L, 1);
            
            if (isArray) {
                QJsonArray arr;
                int len = lua_rawlen(L, index);
                for (int i = 1; i <= len; ++i) {
                    lua_rawgeti(L, index, i);
                    arr.append(lua_to_json(L, lua_gettop(L)));
                    lua_pop(L, 1);
                }
                return arr;
            } else {
                QJsonObject obj;
                lua_pushnil(L);
                while (lua_next(L, index) != 0) {
                    const char* key = lua_tostring(L, -2);
                    if (key) {
                        obj.insert(key, lua_to_json(L, lua_gettop(L)));
                    }
                    lua_pop(L, 1);
                }
                return obj;
            }
        }
        default: return QJsonValue();
    }
}

void LuaWorker::json_to_lua(lua_State *L, const QJsonValue &val)
{
    if (val.isBool()) lua_pushboolean(L, val.toBool());
    else if (val.isDouble()) lua_pushnumber(L, val.toDouble());
    else if (val.isString()) lua_pushstring(L, val.toString().toUtf8().constData());
    else if (val.isArray()) {
        lua_newtable(L);
        QJsonArray arr = val.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            json_to_lua(L, arr[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else if (val.isObject()) {
        lua_newtable(L);
        QJsonObject obj = val.toObject();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            lua_pushstring(L, it.key().toUtf8().constData());
            json_to_lua(L, it.value());
            lua_settable(L, -3);
        }
    } else {
        lua_pushnil(L);
    }
}

void LuaWorker::processTranslation(const QString &sourceText)
{
    if (!L) {
        if (!initLua()) return;
    }

    lua_getglobal(L, "on_text_extract");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        emit translationFinished({sourceText, sourceText});
        return;
    }

    lua_pushstring(L, sourceText.toUtf8().constData());

    // Expect 2 return values: result, error_message
    if (lua_pcall(L, 1, 2, 0) != LUA_OK) {
        QString error = lua_tostring(L, -1);
        qCritical() << "Error calling on_text_extract:" << error;
        emit errorOccurred(QString("Lua execution error: %1").arg(error));
        lua_pop(L, 1);
        return;
    }

    // Check first return value (translation)
    if (lua_isnil(L, -2)) {
        QString errorMsg = "Translation failed (Script returned nil)";
        // Check second return value (error message)
        if (lua_isstring(L, -1)) {
            errorMsg = QString::fromUtf8(lua_tostring(L, -1));
        }
        lua_pop(L, 2); // Pop both results
        emit errorOccurred(errorMsg);
        return;
    }

    QString result = sourceText;
    if (lua_isstring(L, -2)) {
        result = QString::fromUtf8(lua_tostring(L, -2));
    }
    lua_pop(L, 2); // Pop both results

    emit translationFinished({sourceText, result});
}

void LuaWorker::processBatchTranslation(const QStringList &sourceTexts)
{
    if (!L) {
        if (!initLua()) return;
    }

    lua_getglobal(L, "on_batch_text_extract");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        // Fallback to sequential? Or just error?
        // Let's fallback to sequential for compatibility, but it defeats the purpose of batching.
        // Better to log a warning and do sequential.
        qWarning() << "Script does not support on_batch_text_extract. Falling back to sequential.";
        
        QList<qtlingo::TranslationResult> results;
        for (const QString &text : sourceTexts) {
             // We can't easily reuse processTranslation because it emits signals.
             // We should probably just emit error or implement a loop here.
             // For now, let's just emit error to encourage implementing the function.
             // Or better: implement a loop and emit batchFinished at the end.
             
             // Actually, let's just try to call on_text_extract in a loop.
             lua_getglobal(L, "on_text_extract");
             if (lua_isfunction(L, -1)) {
                 lua_pushstring(L, text.toUtf8().constData());
                 if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
                     QString res = text;
                     if (lua_isstring(L, -1)) res = QString::fromUtf8(lua_tostring(L, -1));
                     results.append({text, res});
                 } else {
                     results.append({text, text}); // Error fallback
                 }
                 lua_pop(L, 1); // Pop result
             } else {
                 lua_pop(L, 1);
                 results.append({text, text});
             }
        }
        emit batchTranslationFinished(results);
        return;
    }

    // Prepare table
    lua_newtable(L);
    for (int i = 0; i < sourceTexts.size(); ++i) {
        lua_pushstring(L, sourceTexts[i].toUtf8().constData());
        lua_rawseti(L, -2, i + 1);
    }

    // Call function
    // Expect 2 return values: result_table, error_message
    if (lua_pcall(L, 1, 2, 0) != LUA_OK) {
        QString error = lua_tostring(L, -1);
        qCritical() << "Error calling on_batch_text_extract:" << error;
        emit errorOccurred(QString("Lua execution error: %1").arg(error));
        lua_pop(L, 1);
        return;
    }

    // Check first return value (table)
    if (lua_isnil(L, -2)) {
        QString errorMsg = "Batch Translation failed (Script returned nil)";
        if (lua_isstring(L, -1)) {
            errorMsg = QString::fromUtf8(lua_tostring(L, -1));
        }
        lua_pop(L, 2);
        emit errorOccurred(errorMsg);
        return;
    }

    QList<qtlingo::TranslationResult> results;
    if (lua_istable(L, -2)) {
        int len = lua_rawlen(L, -2);
        // We expect the returned array to match the input array order
        for (int i = 1; i <= len; ++i) {
            lua_rawgeti(L, -2, i);
            QString res = "";
            if (lua_isstring(L, -1)) {
                res = QString::fromUtf8(lua_tostring(L, -1));
            }
            lua_pop(L, 1);
            
            if (i - 1 < sourceTexts.size()) {
                results.append({sourceTexts[i-1], res});
            }
        }
    }
    lua_pop(L, 2); // Pop table and error (or nil)

    emit batchTranslationFinished(results);
}

// --- LuaTranslationService Implementation ---

LuaTranslationService::LuaTranslationService(const QString &scriptPath, QObject *parent)
    : qtlingo::ITranslationService(parent)
{
    QFileInfo fi(scriptPath);
    m_serviceName = "Lua: " + fi.fileName();

    m_worker = new LuaWorker(scriptPath);
    m_worker->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(this, &LuaTranslationService::startTranslation, m_worker, &LuaWorker::processTranslation);
    connect(this, &LuaTranslationService::startBatchTranslation, m_worker, &LuaWorker::processBatchTranslation);
    connect(m_worker, &LuaWorker::translationFinished, this, &LuaTranslationService::translationFinished);
    connect(m_worker, &LuaWorker::batchTranslationFinished, this, &LuaTranslationService::batchTranslationFinished);
    connect(m_worker, &LuaWorker::errorOccurred, this, &LuaTranslationService::errorOccurred);
    connect(m_worker, &LuaWorker::logMessage, this, &LuaTranslationService::logMessage); // Connect log signal

    m_workerThread.start();
}

LuaTranslationService::~LuaTranslationService()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

QString LuaTranslationService::serviceName() const
{
    return m_serviceName;
}

void LuaTranslationService::translate(const QString &sourceText)
{
    emit startTranslation(sourceText);
}

void LuaTranslationService::batchTranslate(const QStringList &sourceTexts)
{
    emit startBatchTranslation(sourceTexts);
}
