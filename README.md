# 🏠 Raspberry Pi Pico W Smart Home Dashboard

A Smart Home automation project built using the **Raspberry Pi Pico W**. This project allows you to control a relay through both a **Telegram Bot** and a **Local Web Dashboard**.

---

## ✨ Features

- 🌐 Modern Dark Web Dashboard
- 🤖 Telegram Bot with Inline Keyboard
- 🟢 NO / 🔴 NC Relay Control
- 📊 Live Relay Status
- 🔄 AJAX Auto Refresh
- 📡 Wi-Fi Connectivity
- ⏰ NTP Time Synchronization
- 🔒 Telegram Chat ID Authentication
- 📱 Mobile Friendly Interface

---

## 🛠 Hardware Used

- Raspberry Pi Pico W
- 1 Channel Relay Module
- 2-Way Switch (SPDT)
- AC Light / Motor
- 5V Power Supply

---

## 🔌 Hardware Connections

### Raspberry Pi Pico W → Relay Module

| Pico W | Relay Module |
|---------|--------------|
| GP1 | IN |
| VBUS (5V) | VCC |
| GND | GND |

### AC Wiring

```text
           AC 230V

Phase (L)
    │
    ▼
Relay COM
 ┌─────┴─────┐
 │           │
NO          NC
 │           │
L1          L2
   2-Way Switch
        │
       COM
        │
 Light / Motor
        │
 Neutral (N)
```

---

## 🤖 Telegram Dashboard

```
┌──────────────┬──────────────┐
│ 🟢 NO        │ 🔴 NC        │
├──────────────┼──────────────┤
│ 📊 STATUS    │ 🔄 REFRESH   │
├──────────────┴──────────────┤
│ 🌐 OPEN WEB DASHBOARD       │
└─────────────────────────────┘
```

---

## 🌐 Web Dashboard

- 🟢 NO Button
- 🔴 NC Button
- 📊 Status
- 🔄 Refresh
- Responsive Dark UI
- Live Relay Status

---

## 📚 Libraries

- WiFi
- WiFiClientSecure
- UniversalTelegramBot
- ArduinoJson

---

## 🚀 Getting Started

1. Install the required libraries.
2. Update your Wi-Fi SSID and Password.
3. Update your Telegram Bot Token and Chat ID.
4. Upload the sketch to the Raspberry Pi Pico W.
5. Open the Web Dashboard using the Pico W IP address.
6. Send `/start` to the Telegram Bot.

---

## 📄 License

This project is licensed under the **MIT License**.

---

## 👨‍💻 Author

**Thamizharasu M**

GitHub:
https://github.com/MVS-Thamizharasu
