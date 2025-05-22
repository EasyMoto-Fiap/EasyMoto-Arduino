import serial
from datetime import datetime

serial_port = 'COM5'
baudrate = 9600

try:
    ser = serial.Serial(serial_port, baudrate, timeout=1)
    print(f"Conectado ao Arduino em {serial_port} (baudrate {baudrate})")
    print("EasyMoto - Monitoramento Serial (pressione Ctrl+C para sair)\n")
except Exception as e:
    print(f"Erro ao abrir a porta serial: {e}")
    exit(1)

leituras = []

try:
    while True:
        line = ser.readline().decode(errors='ignore').strip()
        if line:
            agora = datetime.now().strftime("%H:%M:%S")
            print(f"[{agora}] {line}")
            leituras.append((agora, line))
except KeyboardInterrupt:
    print("\nEncerrando leitura serial.")
finally:
    ser.close()


