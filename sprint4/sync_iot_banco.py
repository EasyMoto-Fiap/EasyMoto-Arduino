import requests
import oracledb

# -----------------------------
# ThingSpeak
# -----------------------------
CHANNEL_ID = 3154894  
READ_API_KEY = "SUA_READ_API_KEY_AQUI" 

TS_URL = f"https://api.thingspeak.com/channels/{CHANNEL_ID}/feeds.json"

# -----------------------------
# Banco Oracle (FIAP)
# -----------------------------
DB_USER = "rm557177"
DB_PASSWORD = "170403"
DB_DSN = "oracle.fiap.com.br:1521/ORCL" 

def get_last_by_moto():
    params = {
        "api_key": READ_API_KEY,
        "results": 100 
    }
    r = requests.get(TS_URL, params=params, timeout=10)
    print("status:", r.status_code)
    print("body:", r.text)
    r.raise_for_status()

    data = r.json()
    feeds = data.get("feeds", [])

    last = {}  # moto_id -> status_codigo

    for f in feeds:
        field1 = f.get("field1")  # StatusCodigo
        field3 = f.get("field3")  # MotoId

        if field1 is None or field3 is None:
            continue

        try:
            status_codigo = int(field1)
            moto_id = int(field3)
        except ValueError:
            continue
        last[moto_id] = status_codigo

    return last

def update_db(last_by_moto):
    if not last_by_moto:
        print("Nenhum dado valido do ThingSpeak.")
        return

    conn = oracledb.connect(user=DB_USER, password=DB_PASSWORD, dsn=DB_DSN)
    cur = conn.cursor()

    for moto_id, status_codigo in last_by_moto.items():
        if status_codigo < 1 or status_codigo > 3:
            continue

        novo_status = status_codigo

        print(f"Atualizando Moto {moto_id}: status_moto = {novo_status}")
        cur.execute(
            "UPDATE Moto SET status_moto = :1 WHERE id_moto = :2",
            [novo_status, moto_id]
        )

    conn.commit()
    cur.close()
    conn.close()
    print("OK.")

def main():
    info = get_last_by_moto()
    print("Ultimos por moto:", info)
    update_db(info)

if __name__ == "__main__":
    main()
