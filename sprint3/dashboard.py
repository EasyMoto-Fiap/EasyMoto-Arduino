import json, csv, time
from datetime import datetime, timezone
from paho.mqtt.client import Client
from rich.live import Live
from rich.table import Table

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "mottu/bike/status/#"
CSV_PATH = "log_status.csv"
OFFLINE_S = 30

devices = {} 

def render():
    t = Table(title="Status das Motos (MQTT)")
    t.add_column("ID"); t.add_column("Status"); t.add_column("ts (UTC)"); t.add_column("Recebido"); t.add_column("Sem dados (s)")
    now = time.time()
    for did, d in sorted(devices.items()):
        gap = now - d["last_rx_epoch"]
        status = d["status"] + ("  [OFFLINE]" if gap > OFFLINE_S else "")
        t.add_row(did, status, d["ts"], d["last_rx_local"], f"{gap:.0f}")
    return t

def on_message(_c, _u, msg):
    try:
        data = json.loads(msg.payload.decode())
        did = str(data.get("id", "?"))
        st  = str(data.get("status", "?"))
        ts  = str(data.get("ts", ""))
        try:
            ts_dt = datetime.fromisoformat(ts.replace("Z","+00:00"))
        except Exception:
            ts_dt = None
        devices[did] = {
            "status": st,
            "ts": ts,
            "ts_dt": ts_dt,
            "last_rx_local": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            "last_rx_epoch": time.time(),
        }
        with open(CSV_PATH, "a", newline="") as f:
            csv.writer(f).writerow([datetime.now().isoformat(), did, st, ts])
    except Exception as e:
        print("Erro payload:", e)

if __name__ == "__main__":
    cli = Client()
    cli.connect(BROKER, PORT, 60)
    cli.subscribe(TOPIC)
    cli.on_message = on_message
    cli.loop_start()
    try:
        with Live(render(), refresh_per_second=4) as live:
            while True:
                live.update(render())
                time.sleep(0.25)
    except KeyboardInterrupt:
        pass
