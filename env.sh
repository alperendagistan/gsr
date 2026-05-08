#!/bin/bash
# env.sh - NCS toolchain ortam değişkenlerini ayarlar
# Diğer scriptler tarafından source edilir. Doğrudan çalıştırmayın.
#
# Override'lar (isterseniz export edin):
#   NCS_BASE         - NCS kurulum dizini (default: ~/ncs)
#   ZEPHYR_WORKSPACE - west workspace dizini (default: ~/zephyrproject)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export PROJECT_DIR="$SCRIPT_DIR"

# --- NCS Toolchain ---
NCS_BASE="${NCS_BASE:-$HOME/ncs}"
if [ ! -d "$NCS_BASE/toolchains" ]; then
    echo "HATA: NCS toolchain bulunamadı: $NCS_BASE/toolchains"
    echo "      NCS_BASE değişkenini ayarlayın."
    return 1 2>/dev/null || exit 1
fi

# En son toolchain'i seç
TC=$(find "$NCS_BASE/toolchains" -maxdepth 1 -mindepth 1 -type d | sort | tail -1)
if [ -z "$TC" ]; then
    echo "HATA: Geçerli toolchain bulunamadı: $NCS_BASE/toolchains/"
    return 1 2>/dev/null || exit 1
fi

export PYTHONHOME="$TC/usr/local"
export PYTHONPATH="$TC/usr/local/lib/python3.12:$TC/usr/local/lib/python3.12/site-packages"
export PATH="$TC/bin:$TC/usr/bin:$TC/usr/local/bin:$TC/opt/bin:$TC/nrfutil/bin:$TC/opt/zephyr-sdk/arm-zephyr-eabi/bin:$TC/opt/zephyr-sdk/riscv64-zephyr-elf/bin:$PATH"
export LD_LIBRARY_PATH="$TC/lib:$TC/lib/x86_64-linux-gnu:$TC/usr/local/lib"

# --- Zephyr Workspace ---
ZEPHYR_WORKSPACE="${ZEPHYR_WORKSPACE:-$HOME/zephyrproject}"
if [ ! -d "$ZEPHYR_WORKSPACE/.west" ]; then
    echo "HATA: West workspace bulunamadı: $ZEPHYR_WORKSPACE/.west"
    echo "      ZEPHYR_WORKSPACE değişkenini ayarlayın."
    return 1 2>/dev/null || exit 1
fi

export ZEPHYR_BASE="$ZEPHYR_WORKSPACE/zephyr"
export WEST_WORKSPACE="$ZEPHYR_WORKSPACE"

# --- Board Tanımları ---
export BOARD_NRF54L="nrf54l15dk/nrf54l15/cpuapp"
export BOARD_NRF52="nrf52840dk/nrf52840"
export BOARD="${BOARD:-$BOARD_NRF54L}"

echo "--- Ortam Hazır ---"
echo "  Toolchain: $TC"
echo "  Workspace: $ZEPHYR_WORKSPACE"
echo "  Board:     $BOARD"
echo "  Proje:     $PROJECT_DIR"
echo "-------------------"
