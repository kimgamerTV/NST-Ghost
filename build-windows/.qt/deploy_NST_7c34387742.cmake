include("/home/jop/work/NST/NST/build-windows/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/NST-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/jop/work/NST/NST/build-windows/NST.exe"
    GENERATE_QT_CONF
)
