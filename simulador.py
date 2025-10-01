import time, json, random
from datetime import datetime
import paho.mqtt.client as mqtt

BROKER = "broker.hivemq.com"; PORT = 1883
IDS = ["moto-02", "moto-03"]
ESTADOS = ["PRONTA", "PENDENTE", "MANUTENCAO"]

try:
    cli = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2)
    API_V2 = True
except Exception:
    cli = mqtt.Client()
    API_V2 = False

if API_V2:
    def on_connect(client, userdata, flags, reason_code, properties):
        print(f"[MQTT] Connected rc={reason_code}")
    def on_disconnect(client, userdata, reason_code, properties):
        print(f"[MQTT] Disconnected rc={reason_code}")
    def on_publish(client, userdata, mid, reason_code, properties):
        print(f"[MQTT] Published mid={mid} rc={reason_code}")
else:
    def on_connect(client, userdata, flags, rc):
        print(f"[MQTT] Connected rc={rc}")
    def on_disconnect(client, userdata, rc):
        print(f"[MQTT] Disconnected rc={rc}")
    def on_publish(client, userdata, mid):
        print(f"[MQTT] Published mid={mid}")

cli.on_connect = on_connect
cli.on_disconnect = on_disconnect
cli.on_publish = on_publish

cli.connect(BROKER, PORT, 60)
cli.loop_start()

try:
    while True:
        for did in IDS:
            payload = {
                "id": did,
                "status": random.choice(ESTADOS),
                "ts": datetime.utcnow().isoformat(timespec="seconds") + "Z"
            }
            topic = f"mottu/bike/status/{did}"
            print(f"[PUB] {topic} <- {payload}")
            cli.publish(topic, json.dumps(payload), retain=True)
        time.sleep(3)
except KeyboardInterrupt:
    pass
