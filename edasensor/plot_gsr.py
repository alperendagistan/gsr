import glob
import time
import math
import threading
from collections import deque

import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

BAUD = 115200
WINDOW_SECONDS = 120
MAX_POINTS = 2000
Y_STEP = 50

# EMA parametresi
EMA_ALPHA = 0.15

# 1D Kalman parametreleri
# process_variance: sinyalin gerçek değişimi ne kadar hızlı olabilir
# measurement_variance: ölçüm gürültüsü ne kadar büyük
KALMAN_Q = 1.0
KALMAN_R = 25.0

times = deque(maxlen=MAX_POINTS)
raw_values = deque(maxlen=MAX_POINTS)
ema_values = deque(maxlen=MAX_POINTS)
kalman_values = deque(maxlen=MAX_POINTS)
delta_values = deque(maxlen=MAX_POINTS)

lock = threading.Lock()
start_time_ms = None


class Kalman1D:
    def __init__(self, q=1.0, r=25.0):
        self.q = q
        self.r = r
        self.x = None
        self.p = 1.0

    def update(self, measurement: float) -> float:
        if self.x is None:
            self.x = measurement
            return self.x

        # predict
        self.p = self.p + self.q

        # update
        k = self.p / (self.p + self.r)
        self.x = self.x + k * (measurement - self.x)
        self.p = (1 - k) * self.p

        return self.x


kalman = Kalman1D(q=KALMAN_Q, r=KALMAN_R)
ema_state = None
last_filtered = None


def find_port():
    ports = sorted(glob.glob("/dev/ttyACM1"))
    if not ports:
        return None
    return ports[0]


def apply_ema(value: float) -> float:
    global ema_state

    if ema_state is None:
        ema_state = value
    else:
        ema_state = EMA_ALPHA * value + (1.0 - EMA_ALPHA) * ema_state

    return ema_state


def serial_reader():
    global start_time_ms, last_filtered

    while True:
        port = find_port()
        if not port:
            print("No /dev/ttyACM* device found, retrying...")
            time.sleep(1)
            continue

        try:
            print(f"Connecting to {port} ...")
            with serial.Serial(port, BAUD, timeout=1) as ser:
                print(f"Connected to {port}")

                while True:
                    line = ser.readline().decode(errors="ignore").strip()
                    if not line:
                        continue

                    parts = line.split(",")
                    if len(parts) != 2:
                        continue

                    try:
                        t_ms = int(parts[0].strip())
                        raw = int(parts[1].strip())
                    except ValueError:
                        continue

                    if start_time_ms is None:
                        start_time_ms = t_ms

                    rel_t = (t_ms - start_time_ms) / 1000.0

                    ema_val = apply_ema(raw)
                    kalman_val = kalman.update(raw)

                    if last_filtered is None:
                        delta = 0.0
                    else:
                        delta = kalman_val - last_filtered

                    last_filtered = kalman_val

                    with lock:
                        times.append(rel_t)
                        raw_values.append(raw)
                        ema_values.append(ema_val)
                        kalman_values.append(kalman_val)
                        delta_values.append(delta)

        except serial.SerialException as e:
            print(f"Serial error: {e}")
            print("Reconnecting...")
            time.sleep(1)
        except Exception as e:
            print(f"Unexpected error: {e}")
            time.sleep(1)


def rounded_limits(values, step=50, min_span=50):
    vmin = min(values)
    vmax = max(values)

    if vmax - vmin < min_span:
        center = (vmin + vmax) / 2.0
        vmin = center - min_span / 2
        vmax = center + min_span / 2

    low = math.floor(vmin / step) * step
    high = math.ceil(vmax / step) * step

    if low == high:
        high += step

    return low, high


def update(_frame):
    with lock:
        if not times:
            ax1.clear()
            ax2.clear()
            ax1.set_title("Live GSR Signal")
            ax1.set_ylabel("ADC Value")
            ax1.grid(True)

            ax2.set_title("Fast Change / Phasic-like Activity")
            ax2.set_xlabel("Time (s)")
            ax2.set_ylabel("Delta")
            ax2.grid(True)
            return

        x = list(times)
        raw = list(raw_values)
        ema = list(ema_values)
        kal = list(kalman_values)
        delta = list(delta_values)

    x_max = x[-1]
    x_min = max(0, x_max - WINDOW_SECONDS)

    visible_idx = [i for i, xv in enumerate(x) if xv >= x_min]
    if not visible_idx:
        return

    x_vis = [x[i] for i in visible_idx]
    raw_vis = [raw[i] for i in visible_idx]
    ema_vis = [ema[i] for i in visible_idx]
    kal_vis = [kal[i] for i in visible_idx]
    delta_vis = [delta[i] for i in visible_idx]

    ax1.clear()
    ax2.clear()

    ax1.plot(x_vis, raw_vis, label="Raw")
    ax1.plot(x_vis, ema_vis, label="EMA")
    ax1.plot(x_vis, kal_vis, label="Kalman")

    y1_low, y1_high = rounded_limits(raw_vis + ema_vis + kal_vis, step=Y_STEP, min_span=50)
    ax1.set_xlim(x_min, x_min + WINDOW_SECONDS)
    ax1.set_ylim(y1_low, y1_high)
    ax1.set_yticks(range(int(y1_low), int(y1_high) + 1, Y_STEP))
    ax1.set_title("Live GSR Signal")
    ax1.set_ylabel("ADC Value")
    ax1.grid(True)
    ax1.legend(loc="upper right")

    ax2.plot(x_vis, delta_vis, label="Delta (Kalman diff)")
    d_abs = max(abs(min(delta_vis)), abs(max(delta_vis)), 1)
    d_lim = max(5, math.ceil(d_abs / 5) * 5)
    ax2.set_xlim(x_min, x_min + WINDOW_SECONDS)
    ax2.set_ylim(-d_lim, d_lim)
    ax2.set_title("Fast Change / Phasic-like Activity")
    ax2.set_xlabel("Time (s)")
    ax2.set_ylabel("Delta")
    ax2.grid(True)

    # İsteğe bağlı basit threshold çizgileri
    ax2.axhline(2, linestyle="--")
    ax2.axhline(-2, linestyle="--")


threading.Thread(target=serial_reader, daemon=True).start()

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8), sharex=True)
ani = FuncAnimation(fig, update, interval=100, cache_frame_data=False)
plt.tight_layout()
plt.show()
