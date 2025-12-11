include("/home/jop/work/NST/NST/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/NST-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/jop/work/NST/NST/NST"
    GENERATE_QT_CONF
)
