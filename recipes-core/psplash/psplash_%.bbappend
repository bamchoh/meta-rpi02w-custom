FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SPLASH_IMAGES:rpi = "file://psplash-pidrive-img.h;outsuffix=pidrive"

do_install:append:rpi() {
    install -m 0755 ${B}/psplash ${D}${bindir}/psplash
}
