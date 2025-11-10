import csv
import time
import requests

TS_SERVER  = "https://api.thingspeak.com/update"
TS_API_KEY = "RVFSGCR8NTA1RSII"  

ORIGEM = "CSV_SIM"

status_to_codigo = {
    "PRONTA": 0,
    "PENDENTE": 1,
    "MANUTENCAO": 2,
}

def main():
    with open("log_status.csv", newline="", encoding="utf-8") as f:
        reader = csv.reader(f)

        for i, row in enumerate(reader):
            ts_local, moto_label, status_txt, ts_cloud = row

            # "moto-01" -> 1
            moto_id = int(moto_label.split("-")[1])
            codigo = status_to_codigo[status_txt]

            print(f"Linha {i}: {moto_label}  status={status_txt} -> codigo={codigo}  moto_id={moto_id}")

            params = {
                "api_key": TS_API_KEY,
                "field1": codigo,
                "field2": status_txt,
                "field3": moto_id,
                "field4": ORIGEM,
            }

            r = requests.get(TS_SERVER, params=params, timeout=10)
            print("Enviado:", params, "HTTP:", r.status_code, r.text)
            print("-" * 40)

            time.sleep(16) 

if __name__ == "__main__":
    main()
