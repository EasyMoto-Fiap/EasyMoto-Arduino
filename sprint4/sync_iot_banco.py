import requests
import oracledb
from datetime import datetime
import time

CHANNEL_ID = 3154894  
READ_API_KEY = "RVFSGCR8NTA1RSII"
TS_URL = f"https://api.thingspeak.com/channels/{CHANNEL_ID}/feeds.json"

DB_USER = "rm557177"
DB_PASSWORD = "170403"
DB_DSN = "oracle.fiap.com.br:1521/ORCL"

def get_thingspeak_data():
    try:
        params = {
            "api_key": READ_API_KEY,
            "results": 100
        }
        print(f"\n[{datetime.now().strftime('%H:%M:%S')}] 🔄 Buscando ThingSpeak...")
        r = requests.get(TS_URL, params=params, timeout=10)
        
        if r.status_code != 200:
            print(f"❌ ThingSpeak error: {r.status_code}")
            return {}
        
        data = r.json()
        feeds = data.get("feeds", [])
        print(f"✓ {len(feeds)} feeds recebidos")
        
        last_data = {}
        for f in feeds:
            field1 = f.get("field1")
            field3 = f.get("field3")
            
            if field1 is None or field3 is None:
                continue
            
            try:
                status_codigo = int(float(field1))
                moto_id = int(float(field3))
            except (ValueError, TypeError):
                continue
            
            if status_codigo == 0:
                continue
            
            if moto_id not in last_data:
                last_data[moto_id] = status_codigo
        
        print(f"✓ Dados processados: {last_data}")
        return last_data
        
    except Exception as e:
        print(f"❌ Erro ThingSpeak: {e}")
        return {}

def update_banco(dados):
    if not dados:
        print("⚠ Sem dados para atualizar")
        return False
    
    conn = None
    try:
        print(f"[{datetime.now().strftime('%H:%M:%S')}] 📝 Conectando Oracle...")
        conn = oracledb.connect(user=DB_USER, password=DB_PASSWORD, dsn=DB_DSN)
        cur = conn.cursor()
        print("✓ Conectado ao Oracle")
        
        for moto_id, status_codigo in dados.items():
            print(f"\n  → Atualizando Moto {moto_id} para Status {status_codigo}...")
            
            sql = "UPDATE MOTO SET STATUS_MOTO = :status WHERE ID_MOTO = :moto_id"
            cur.execute(sql, {"status": status_codigo, "moto_id": moto_id})
            
            linhas_afetadas = cur.rowcount
            print(f"    ✓ Linhas afetadas: {linhas_afetadas}")
            
            if linhas_afetadas > 0:
                conn.commit()
                print(f"    ✓ COMMIT realizado!")
                cur.execute("SELECT STATUS_MOTO FROM MOTO WHERE ID_MOTO = :moto_id", {"moto_id": moto_id})
                resultado = cur.fetchone()
                if resultado:
                    status_banco = resultado[0]
                    print(f"    ✓ Verificação: Banco tem Status = {status_banco}")
                    if status_banco == status_codigo:
                        print(f"    ✅ CONFIRMADO: Moto {moto_id} atualizada com sucesso!")
                    else:
                        print(f"    ❌ ERRO: Status no banco é {status_banco}, esperava {status_codigo}")
            else:
                print(f"    ❌ Moto {moto_id} não encontrada no banco!")
        
        cur.close()
        print("\n✓ Todas as atualizações concluídas!")
        return True
        
    except oracledb.DatabaseError as e:
        print(f"❌ Erro Oracle: {e}")
        if conn:
            conn.rollback()
        return False
    except Exception as e:
        print(f"❌ Erro inesperado: {e}")
        if conn:
            conn.rollback()
        return False
    finally:
        if conn:
            conn.close()

def main():
    print("="*70)
    print("🚀 SINCRONIZAÇÃO ThingSpeak → Oracle (COM DEBUG)")
    print("="*70)
    dados = get_thingspeak_data()
    if dados:
        update_banco(dados)
    
    print("\n" + "="*70)
    print("⏸ Aguardando 30s para próxima sincronização...")
    print("="*70)

    contador = 0
    while True:
        try:
            time.sleep(30)
            contador += 1
            print(f"\n\n{'='*70}")
            print(f"📍 Sincronização #{contador}")
            print(f"{'='*70}")
            
            dados = get_thingspeak_data()
            if dados:
                update_banco(dados)
            
            print("\n⏸ Aguardando 30s para próxima sincronização...")
            
        except KeyboardInterrupt:
            print("\n\n⏹ Sincronização parada pelo usuário.")
            break
        except Exception as e:
            print(f"\n❌ Erro no loop: {e}")
            time.sleep(5)

if __name__ == "__main__":
    main()