TEMPLATE = subdirs
SUBDIRS += app/deepin-clone-app.pro

isEmpty(DISABLE_GUI) {
    SUBDIRS += honest_child
}

isEmpty(DISABLE_DFM_PLUGIN) {
    load(configure)
    qtCompileTest(dfm-plugin) {
#        SUBDIRS += dfm_plugin
#        message(Enable DDE File Manager Plugin)
    } else {
        warning(Disable DDE File Manager Plugin)
    }
}
