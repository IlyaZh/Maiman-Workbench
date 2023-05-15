TEMPLATE = aux

CONFIG += file_copies
COPIES += core translations styles platforms imageformats iconengines bearer

CONFIG(release, debug|release) {
    DESTDIR_WIN = $$PWD/packages/com.maimanelectronics.MB/data
    DESTDIR_WIN ~= s,/,\\,g
    PWD_WIN = $$OUT_PWD/../MB_Release
    PWD_WIN ~= s,/,\\,g

    core.path = $$DESTDIR_WIN
    translations.path = $$DESTDIR_WIN/translations
    styles.path = $$DESTDIR_WIN/styles
    platforms.path = $$DESTDIR_WIN/platforms
    imageformats.path = $$DESTDIR_WIN/imageformats
    iconengines.path = $$DESTDIR_WIN/iconengines
    bearer.path = $$DESTDIR_WIN/bearer

    core.files += $$files($$PWD_WIN/*.dll)
    core.files += $$files($$PWD_WIN/*.xml)
    core.files += $$files($$PWD_WIN/*.exe)
    bearer.files = $$files($$PWD_WIN/bearer/*.dll)
    iconengines.files = $$files($$PWD_WIN/iconengines/*.dll)
    imageformats.files = $$files($$PWD_WIN/imageformats/*.dll)
    platforms.files = $$files($$PWD_WIN/platforms/*.dll)
    styles.files = $$files($$PWD_WIN/styles/*.dll)
    translations.files = $$files($$PWD_WIN/translations/*.qm)

    INSTALLER_OFFLINE = $$OUT_PWD/../installer_release/MB.offline

    INPUT = $$PWD/config/config.xml $$PWD/packages
#    offlineInstaller.depends = core translations styles platforms imageformats iconengines bearer# copydata
    offlineInstaller.input = INPUT
    offlineInstaller.output = $$INSTALLER_OFFLINE
#    offlineInstaller.commands = $$(QTDIR)\..\..\Tools\QtInstallerFramework\4.2\bin\binarycreator --offline-only -c $$PWD/config/config.xml -p $$PWD/packages ${QMAKE_FILE_OUT}
    offlineInstaller.commands = C:\Qt5\Tools\QtInstallerFramework\4.2\bin\binarycreator --offline-only -c $$PWD/config/config.xml -p $$PWD/packages ${QMAKE_FILE_OUT}
    offlineInstaller.CONFIG += target_predeps no_link combine
    QMAKE_EXTRA_COMPILERS += offlineInstaller
}

DISTFILES += \
    config/config.xml \
    packages/com.maimanelectronics.MB/meta/package.xml

