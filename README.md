# 🛵 EasyMoto IoT — Gestão Inteligente de Motos no Pátio

---

## ❓ Qual problema real o projeto resolve?

A gestão manual da localização e status das motos em pátios de locadoras causa atrasos, retrabalho, riscos de segurança e baixa produtividade.  
**A falta de padronização dificulta o monitoramento em tempo real**, aumenta os erros e prejudica a operação, especialmente em empresas com múltiplas filiais e grande volume de movimentações diárias.

---

## 🎬 Pitch do Projeto

👉 [Pitch aqui!](LINK_DO_VIDEO_AQUI)

---

## 💡 Justificativa

A aplicação de **IoT** permite **automatizar a identificação e o status das motos**, trazendo mais agilidade, confiabilidade e visibilidade para a gestão.  
O uso de **RFID** garante identificação única sem contato físico, enquanto **atuadores visuais (LEDs)** facilitam a operação e o monitoramento local.  
A integração com **Python** permite visualizar, registrar e analisar os dados em tempo real.

---

## 🛠️ Tecnologias Utilizadas

- **Arduino UNO**  
  Microcontrolador central, responsável por processar a leitura dos cartões RFID e acionar os LEDs.
- **Leitor RFID MFRC522**  
  Sensor que identifica cada moto por meio de TAGs/cartões RFID únicos.
- **LEDs (Verde, Amarelo, Vermelho)**  
  Atuam como indicadores visuais do status da moto:  
  🟢 Pronta | 🟡 Pendente | 🔴 Manutenção.
- **Python + PySerial**  
  Recebe os dados do Arduino via Serial, exibe no terminal, e permite registro/histórico das operações.
- **Protoboard, jumpers, resistores**  
  Facilita a montagem organizada.
---

## ⚙️ Funcionamento do Projeto

1. 🏍️ **Moto recebe TAG RFID única**.
2. 👷‍♂️ **Operador movimenta a moto** no pátio e encosta a TAG no **leitor MFRC522**.
3. 🧠 **Arduino lê o UID**, determina o status (ex: Pronta, Pendente, Manutenção) e acende o **LED correspondente**.
4. 💻 **Dados enviados via Serial USB** para o **Python**, que exibe e registra os eventos em tempo real.
5. 🖥️ **Supervisor pode acompanhar tudo no terminal** — pronto para futuras integrações.

---

## 📸 Imagens do protótipo

<div align="center">
  <img src="https://media.discordapp.net/attachments/954097907806642297/1375187262031007825/ino.jpg?ex=6830c671&is=682f74f1&hm=80b3fec2ef5f884c9287dfc1cad2f1a2d9f5b9c0fc8ce34d38c1ca88f6172270&=&format=webp&width=1032&height=476" width="350"/>
  &nbsp;
  <img src="https://media.discordapp.net/attachments/954097907806642297/1375187661878202368/6a6eada5-c531-4e24-93c3-576cb8c56638.jpg?ex=6830c6d0&is=682f7550&hm=878615733b2358dccd66eb807fbf91e1a82b480d3ddd028f721fa77ca1720aff&=&format=webp&width=1032&height=476" width="350"/>
</div>

---

## 📄 Como rodar

1. Monte o circuito conforme as fotos.
2. Suba o código Arduino do EasyMoto.
3. Feche o Serial Monitor da Arduino IDE.
4. Execute o script Python no computador:
   ```bash
   pip install pyserial
   python easymoto_terminal.py
