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

detect_serial() {
    if [ -n "${SERIAL_NUMBER:-}" ]; then
        echo "$SERIAL_NUMBER"
        return
    fi

    mapfile -t devices < <(nrfutil device list | awk '/^[0-9]+$/ {print $1}')
    if [ "${#devices[@]}" -eq 0 ]; then
        echo "HATA: nrfutil cihazi goremedi." >&2
        exit 1
    fi
    if [ "${#devices[@]}" -gt 1 ]; then
        echo "HATA: Birden fazla cihaz bulundu; SERIAL_NUMBER ayarlayin." >&2
        printf '  %s\n' "${devices[@]}" >&2
        exit 1
    fi

    echo "${devices[0]}"
}

erase_application_core() {
    local serial
    serial="$(detect_serial)"

    echo ">> Application core tamamen siliniyor (serial: $serial)..."
    nrfutil device erase --serial-number "$serial" --core application --all
}

case "${1:-all}" in
    app)
        echo ">> Sadece uygulama yükleniyor..."
        west flash -d "$BUILD_DIR" --domain gsr
        ;;
    boot)
        echo ">> Sadece bootloader yükleniyor..."
        erase_application_core
        west flash -d "$BUILD_DIR" --domain mcuboot
        ;;
    all|*)
        echo ">> Bootloader + Uygulama yükleniyor..."
        erase_application_core
        west flash -d "$BUILD_DIR"
        ;;
esac

echo ""
echo "✓ Flash tamamlandı."
