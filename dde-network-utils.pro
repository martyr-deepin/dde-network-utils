TEMPLATE = subdirs

SUBDIRS += $$PWD/dde-network-utils/dde-network-utils.pro \
           $$PWD/tests/dde-network-utils/tst_dde-network-utils.pro

# Automating generation .qm files from .ts files
CONFIG(release, debug|release) {
    !system($$PWD/translate_generation.sh): error("Failed to generate translation")
}
