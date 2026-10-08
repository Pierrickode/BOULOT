SUMMARY = "AppliGPS Open-Source Navigation Software for NXP i.MX8 Yocto"
DESCRIPTION = "Qt-based GPS navigation application using OpenStreetMap and Waypoint management for i.MX8 SoC family"
HOMEPAGE = "https://github.com/appligps/appligps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "qtbase qtdeclarative qtlocation qtpositioning qtquickcontrols2"
RDEPENDS:${PN} = "qtdeclarative-qmlplugins qtlocation-qmlplugins qtpositioning-qmlplugins qtquickcontrols2-qmlplugins"

SRC_URI = "file://AppliGPS-${PV}.tar.gz \
           file://appligps.service"

S = "${WORKDIR}/AppliGPS-${PV}"

inherit cmake_qt5 systemd

SYSTEMD_SERVICE:${PN} = "appligps.service"

do_install:append() {
    install -d ${D}${bindir}
    install -m 0755 AppliGPS ${D}${bindir}/AppliGPS

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_system_unitdir}
        install -m 0644 ${WORKDIR}/appligps.service ${D}${systemd_system_unitdir}
    fi
}

FILES:${PN} += "${bindir}/AppliGPS ${systemd_system_unitdir}/appligps.service"
