#!/bin/bash
# build.sh - Projeyi derler (mcuboot + app, sysbuild)
#
# Kullanım:
#   ./build.sh          # Temiz build (pristine)
#   ./build.sh fast     # Incremental build (sadece değişenler)
set -e

source "$(dirname "$0")/env.sh"

# --- Board Seçimi ---
if [ -z "$BOARD_SELECTED" ]; then
    echo ""
    echo "Board seçin:"
    echo "  1) nRF54L15 ($BOARD_NRF54L)"
    echo "  2) nRF52840 ($BOARD_NRF52)"
    echo ""
    read -rp "Seçiminiz [1]: " BOARD_CHOICE
    case "${BOARD_CHOICE:-1}" in
        1) export BOARD="$BOARD_NRF54L" ;;
        2) export BOARD="$BOARD_NRF52" ;;
        *) echo "Geçersiz seçim!"; exit 1 ;;
    esac
fi
echo ">> Board: $BOARD"

BUILD_DIR="$WEST_WORKSPACE/build"
PRISTINE=""

if [ "${1}" != "fast" ]; then
    PRISTINE="-p always"
    echo ">> Temiz build (pristine)..."
else
    echo ">> Incremental build..."
fi

cd "$WEST_WORKSPACE"
west build $PRISTINE -b "$BOARD" --sysbuild "$PROJECT_DIR" -d "$BUILD_DIR"

# --- Binary çıktıları proje dizinine kopyala ---
# Versiyonu main.h'den oku
V_MAJOR=$(grep '#define APP_VERSION_MAJOR' "$PROJECT_DIR/main.h" | awk '{print $3}')
V_MINOR=$(grep '#define APP_VERSION_MINOR' "$PROJECT_DIR/main.h" | awk '{print $3}')
V_PATCH=$(grep '#define APP_VERSION_PATCH' "$PROJECT_DIR/main.h" | awk '{print $3}')
VERSION="${V_MAJOR}.${V_MINOR}.${V_PATCH}"
if [ "$VERSION" = ".." ]; then
    echo "UYARI: main.h'den versiyon okunamadı, 0.0.0 kullanılıyor."
    VERSION="0.0.0"
fi
OUTPUT_DIR="$PROJECT_DIR/build"
mkdir -p "$OUTPUT_DIR"

echo ">> Versiyon: v$VERSION"

# MCUboot bootloader
if [ -f "$BUILD_DIR/mcuboot/zephyr/zephyr.hex" ]; then
    cp "$BUILD_DIR/mcuboot/zephyr/zephyr.hex" "$OUTPUT_DIR/mcuboot_v${VERSION}.hex"
    cp "$BUILD_DIR/mcuboot/zephyr/zephyr.bin" "$OUTPUT_DIR/mcuboot_v${VERSION}.bin"
fi

# Ana uygulama (imzalı + onaylı)
if [ -f "$BUILD_DIR/gsr/zephyr/zephyr.signed.confirmed.hex" ]; then
    cp "$BUILD_DIR/gsr/zephyr/zephyr.signed.confirmed.hex" "$OUTPUT_DIR/gsr_v${VERSION}_confirmed.hex"
    cp "$BUILD_DIR/gsr/zephyr/zephyr.signed.confirmed.bin" "$OUTPUT_DIR/gsr_v${VERSION}_confirmed.bin"
fi

# Ana uygulama (imzalı, onaysız - OTA update için)
if [ -f "$BUILD_DIR/gsr/zephyr/zephyr.signed.hex" ]; then
    cp "$BUILD_DIR/gsr/zephyr/zephyr.signed.hex" "$OUTPUT_DIR/gsr_v${VERSION}.hex"
    cp "$BUILD_DIR/gsr/zephyr/zephyr.signed.bin" "$OUTPUT_DIR/gsr_v${VERSION}.bin"
fi

# Merged (bootloader + app tek dosya)
if [ -f "$BUILD_DIR/merged.hex" ]; then
    cp "$BUILD_DIR/merged.hex" "$OUTPUT_DIR/gsr_merged_v${VERSION}.hex"
fi

echo ""
echo "✓ Build tamamlandı (v$VERSION)"
echo "  Çıktılar: $OUTPUT_DIR/"
ls -lh "$OUTPUT_DIR"/*v${VERSION}* 2>/dev/null | awk '{print "    " $NF " (" $5 ")"}'
