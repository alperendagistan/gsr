#!/bin/bash
# run.sh - Derle + yükle + log dinle (hepsi bir arada)
#
# Kullanım:
#   ./run.sh            # Temiz build + flash + log
#   ./run.sh fast       # Incremental build + flash + log
#   ./run.sh log        # Sadece log dinle (build/flash yok)
#
# Ortam değişkenleri:
#   SERIAL_PORT - Log portu (default: /dev/ttyACM1)
#   BAUD        - Baud rate (default: 115200)
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SERIAL_PORT="${SERIAL_PORT:-/dev/ttyACM1}"
BAUD="${BAUD:-115200}"

if [ "${1}" = "log" ]; then
    echo ">> Log dinleniyor: $SERIAL_PORT ($BAUD baud)"
    echo "   Çıkış: Ctrl+C"
    stty -F "$SERIAL_PORT" "$BAUD" raw -echo 2>/dev/null
    cat "$SERIAL_PORT"
    exit 0
fi

# Build
"$SCRIPT_DIR/build.sh" "${1:-}"

# Flash
"$SCRIPT_DIR/flash.sh"

# Log dinle
echo ""
echo ">> Log dinleniyor: $SERIAL_PORT ($BAUD baud)"
echo "   Çıkış: Ctrl+C"
echo ""
sleep 0.5
stty -F "$SERIAL_PORT" "$BAUD" raw -echo 2>/dev/null
cat "$SERIAL_PORT"
