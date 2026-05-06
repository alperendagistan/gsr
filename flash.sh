#!/bin/bash
# flash.sh - Firmware'i cihaza yükler
#
# Kullanım:
#   ./flash.sh          # mcuboot + app birlikte yükle
#   ./flash.sh app      # Sadece uygulamayı yükle
#   ./flash.sh boot     # Sadece bootloader'ı yükle
set -e

source "$(dirname "$0")/env.sh"

BUILD_DIR="$WEST_WORKSPACE/build"

if [ ! -d "$BUILD_DIR" ]; then
    echo "HATA: Build dizini bulunamadı: $BUILD_DIR"
    echo "      Önce ./build.sh çalıştırın."
    exit 1
fi

cd "$WEST_WORKSPACE"

case "${1:-all}" in
    app)
        echo ">> Sadece uygulama yükleniyor..."
        west flash -d "$BUILD_DIR" --domain gsr
        ;;
    boot)
        echo ">> Sadece bootloader yükleniyor..."
        west flash -d "$BUILD_DIR" --domain mcuboot
        ;;
    all|*)
        echo ">> Bootloader + Uygulama yükleniyor..."
        west flash -d "$BUILD_DIR"
        ;;
esac

echo ""
echo "✓ Flash tamamlandı."
