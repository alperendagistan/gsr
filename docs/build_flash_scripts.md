# Yazılım Derleme ve Yükleme Scriptleri

Bu doküman proje kökündeki `*.sh` scriptlerinin ne yaptığını ve tipik derleme, flash ve log alma akışlarını açıklar.

## Script Özeti

| Script | Amaç |
| --- | --- |
| `env.sh` | NCS toolchain, Zephyr workspace ve board ortam değişkenlerini hazırlar. Diğer scriptler bunu otomatik `source` eder. |
| `build.sh` | Projeyi sysbuild ile derler. MCUboot ve uygulama birlikte build edilir. |
| `flash.sh` | Build edilmiş firmware'i karta yükler. Varsayılan modda bootloader + uygulama birlikte yüklenir. |
| `run.sh` | Derleme, yükleme ve UART log dinlemeyi tek komutta çalıştırır. |

## Gereksinimler

- NCS toolchain dizini varsayılan olarak `~/ncs/toolchains` altında olmalıdır.
- Zephyr west workspace varsayılan olarak `~/zephyrproject` altında olmalıdır.
- Kart USB/J-Link üzerinden görünmelidir:

```bash
source ./env.sh
nrfutil device list
```

Birden fazla kart bağlıysa flash sırasında kullanılacak cihazı belirtin:

```bash
SERIAL_NUMBER=1057751979 ./flash.sh
```

## Ortam Hazırlama

Normal kullanımda `env.sh` doğrudan çalıştırılmaz; `build.sh` ve `flash.sh` bunu kendileri yükler.

Elle ortam hazırlamak gerekirse:

```bash
source ./env.sh
```

Desteklenen board değişkenleri:

```bash
BOARD_NRF54L="nrf54l15dk/nrf54l15/cpuapp"
BOARD_NRF52="nrf52840dk/nrf52840"
```

Varsayılan board `nrf54l15dk/nrf54l15/cpuapp` değeridir.

## Derleme

Temiz build:

```bash
./build.sh
```

Bu komut board seçimi ister. Varsayılan seçim için `Enter` basılabilir.

Incremental build:

```bash
./build.sh fast
```

Etkileşimsiz kullanım örneği:

```bash
BOARD_SELECTED=1 BOARD=nrf54l15dk/nrf54l15/cpuapp ./build.sh
```

Build çıktıları iki yerde oluşur:

| Konum                   | İçerik                         |
| ----------------------- | ------------------------------ |
| `~/zephyrproject/build` | West/sysbuild ana build dizini |
| `./build`               | Versiyonlanmış kopya çıktılar  |


Örnek çıktı dosyaları:

```text
build/gsr_merged_v1.0.5.hex
build/gsr_v1.0.5.hex
build/gsr_v1.0.5.bin
build/gsr_v1.0.5_confirmed.hex
build/mcuboot_v1.0.5.hex
```

## Yükleme

Bootloader + uygulama birlikte yükleme:

```bash
./flash.sh
```

Bu modda script önce application core'u tamamen siler, sonra `merged.hex` dosyasını yükler. Bu adım, MCUboot bölgesi RRAM korumasına takılmamak için gereklidir.

Sadece uygulamayı yükleme:

```bash
./flash.sh app
```

Bu mod bootloader alanına dokunmaz. Sadece uygulama değiştiyse en hızlı ve en az riskli yöntem budur.

Sadece bootloader yükleme:

```bash
./flash.sh boot
```

Bu mod da önce application core'u siler, sonra MCUboot domain'ini yükler.

## Derle + Yükle + Log Dinle

Tam akış:

```bash
./run.sh
```

Incremental build ile tam akış:

```bash
./run.sh fast
```

Sadece log dinleme:

```bash
./run.sh log
```

Varsayılan log portu ve baud rate:

```text
SERIAL_PORT=/dev/ttyACM1
BAUD=115200
```

Farklı port kullanmak için:

```bash
SERIAL_PORT=/dev/ttyACM0 ./run.sh log
```

## Beklenen UART Logları

ACM1 üzerinden önce MCUboot/Zephyr banner'ları görülür. Ardından uygulama logları gelmelidir:

```text
GSR Application v1.0.5
Shell initialized
BLE: enabled
BLE: advertising as "GSR-Device"
Heart beat task initialized
System ready. Shell on UART, DFU over BLE.
Heart beat task started
gsr>
alive ...
```

Sadece bootloader banner'ı görülüp uygulama logları gelmiyorsa:

- Build'in güncel olduğundan emin olun: `./build.sh`
- Tam erase + flash deneyin: `./flash.sh`
- MCUboot sektör ayarını kontrol edin: `CONFIG_BOOT_MAX_IMG_SECTORS=256`
- Statik partition dosyasının build'de görüldüğünü kontrol edin: `pm_static.yml`

## Sık Hatalar

### Protected RRAMC Region

Hata:

```text
Address 0x00000000 is in a protected RRAMC region
```

Sebep: `merged.hex` bootloader bölgesini de yazmaya çalışır; cihazda eski firmware bu bölgeyi korumaya almış olabilir.

Çözüm: Güncel `flash.sh`, `./flash.sh` ve `./flash.sh boot` öncesinde otomatik erase yapar. Manuel çözüm:

```bash
source ./env.sh
nrfutil device erase --serial-number 1057751979 --core application --all
./flash.sh
```

### Cihaz Bulunamadı

Hata:

```text
HATA: nrfutil cihazi goremedi.
```

Kontrol:

```bash
source ./env.sh
nrfutil device list
ls -l /dev/ttyACM*
```

USB/J-Link bağlantısını, kartın enerjisini ve kabloyu kontrol edin.

### Birden Fazla Cihaz Bağlı

Hata:

```text
HATA: Birden fazla cihaz bulundu; SERIAL_NUMBER ayarlayin.
```

Çözüm:

```bash
SERIAL_NUMBER=<cihaz_serial> ./flash.sh
```
