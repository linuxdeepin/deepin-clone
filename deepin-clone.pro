TEMPLATE = subdirs
SUBDIRS += app/deepin-clone-app.pro

isEmpty(DISABLE_GUI) {
    SUBDIRS += honest_child
}

isEmpty(DISABLE_DFM_PLUGIN) {
    SUBDIRS += dfm_plugin
}
