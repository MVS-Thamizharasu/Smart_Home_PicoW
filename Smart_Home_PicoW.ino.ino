#include <WiFi.h>              
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <time.h> 

// WiFi Credentials - Replace with your own details
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Telegram Bot Credentials - Replace with your own details
#define BOTtoken "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID "YOUR_TELEGRAM_CHAT_ID"

// Relay connected to GP1 (Physical Pin 2)
const int relayPin = 1; 

WiFiServer server(80);
WiFiClientSecure secClient;
UniversalTelegramBot bot(BOTtoken, secClient);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

// Send Dashboard to Telegram
void sendTelegramDashboard(String chatId) {
  String msg = "🏠 *SMART HOME DASHBOARD (Pico W)*\n\n";
  msg += "Current Status: " + String((digitalRead(relayPin) == LOW) ? "🟢 NO (ON)" : "🔴 NC (OFF)") + "\n";
  msg += "IP Address: " + WiFi.localIP().toString() + "\n";

  String keyboardJson = "[";
  keyboardJson += "[{\"text\":\"🟢 NO\", \"callback_data\":\"/no\"}, {\"text\":\"🔴 NC\", \"callback_data\":\"/nc\"}],";
  keyboardJson += "[{\"text\":\"📊 STATUS\", \"callback_data\":\"/status\"}, {\"text\":\"🔄 REFRESH\", \"callback_data\":\"/refresh\"}],";
  keyboardJson += "[{\"text\":\"🌐 OPEN WEB DASHBOARD\", \"url\":\"http://" + WiFi.localIP().toString() + "\"}]";
  keyboardJson += "]";

  bot.sendMessageWithInlineKeyboard(chatId, msg, "Markdown", keyboardJson);
}

// Handle Incoming Telegram Messages
void handleTelegramMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) continue; 

    String text = bot.messages[i].text;
    String type = bot.messages[i].type;

    if (text == "/start") {
      sendTelegramDashboard(chat_id);
    }
    
    if (type == "callback_query") {
      String callback_data = bot.messages[i].text;
      if (callback_data == "/no") {
        digitalWrite(relayPin, LOW); 
        bot.answerCallbackQuery(bot.messages[i].query_id, "Switched to NO 🟢", true);
        sendTelegramDashboard(chat_id);
      }
      else if (callback_data == "/nc") {
        digitalWrite(relayPin, HIGH); 
        bot.answerCallbackQuery(bot.messages[i].query_id, "Switched to NC 🔴", true);
        sendTelegramDashboard(chat_id);
      }
      else if (callback_data == "/status" || callback_data == "/refresh") {
        bot.answerCallbackQuery(bot.messages[i].query_id, "Updating... 📊", false);
        sendTelegramDashboard(chat_id);
      }
    }
  }
}

// Web Server HTML Dashboard Page
const char INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Pico Smart Home</title>
<style>
*{margin:0;padding:0;box-sizing:border-box;font-family:Arial,sans-serif;}
body{background:#0f172a;display:flex;justify-content:center;align-items:center;height:100vh;color:#fff;}
.card{width:360px;background:#1e293b;padding:25px;border-radius:20px;box-shadow:0 0 30px rgba(0,0,0,.5);text-align:center;}
h1{margin-bottom:20px;}.light{font-size:70px;margin:15px;}.status{font-size:24px;margin-bottom:20px;font-weight:bold;}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:15px;}
button{height:60px;font-size:18px;font-weight:bold;border:none;border-radius:12px;cursor:pointer;transition:.3s;}
.no{background:#16a34a;color:white;}.nc{background:#dc2626;color:white;}.statusBtn{background:#2563eb;color:white;}.refresh{background:#f59e0b;color:black;}
button:hover{transform:scale(1.04);}.info{margin-top:20px;padding:15px;background:#111827;border-radius:15px;text-align:left;line-height:30px;font-size:16px;}
.online{color:#22c55e;}.footer{margin-top:20px;color:#94a3b8;font-size:13px;}
</style>
</head>
<body>
<div class="card">
<h1>🏠 SMART HOME</h1>
<div class="light">💡</div>
<div class="status" id="relayStatus">Loading...</div>
<div class="grid">
<button class="no" onclick="relayNO()">🟢 NO</button>
<button class="nc" onclick="relayNC()">🔴 NC</button>
<button class="statusBtn" onclick="getStatus()">📊 STATUS</button>
<button class="refresh" onclick="location.reload()">🔄 REFRESH</button>
</div>
<div class="info">
Pico W : <span class="online">🟢 ONLINE</span><br>
Relay Contact : <span id="contact">NC</span><br>
IP : <span id="ip">Loading...</span>
</div>
<div class="footer">Smart Home Dashboard v1.0</div>
</div>
<script>
function relayNO(){ fetch("/no"); document.getElementById("relayStatus").innerHTML="Relay : NO"; document.getElementById("contact").innerHTML="NO"; }
function relayNC(){ fetch("/nc"); document.getElementById("relayStatus").innerHTML="Relay : NC"; document.getElementById("contact").innerHTML="NC"; }
function getStatus(){
    fetch("/status").then(r=>r.json()).then(data=>{
        document.getElementById("relayStatus").innerHTML="Relay : "+data.relay;
        document.getElementById("contact").innerHTML=data.relay;
        document.getElementById("ip").innerHTML=data.ip;
    });
}
setInterval(getStatus,2000);
window.onload = getStatus;
</script>
</body>
</html>
)rawliteral";

// Web Server Handler for Pico W
void handleWebServer() {
  WiFiClient client = server.available(); // உங்க விருப்பப்படி இப்போ available() மாற்றப்பட்டுள்ளது
  if (!client) return;
  
  unsigned long timeout = millis() + 200;
  while (!client.available() && millis() < timeout) { delay(1); }
  if (!client.available()) { client.stop(); return; }
  
  String req = client.readStringUntil('\r');
  client.flush();
  
  if (req.indexOf("GET /no") >= 0) {
    digitalWrite(relayPin, LOW);
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nOK");
  } else if (req.indexOf("GET /nc") >= 0) {
    digitalWrite(relayPin, HIGH);
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nOK");
  } else if (req.indexOf("GET /status") >= 0) {
    String currentStatus = (digitalRead(relayPin) == LOW) ? "NO" : "NC";
    String json = "{\"relay\":\"" + currentStatus + "\",\"ip\":\"" + WiFi.localIP().toString() + "\"}";
    client.print("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nConnection: close\r\n\r\n" + json);
  } else if (req.indexOf("GET / HTTP") >= 0 || req.indexOf("GET / ") >= 0) {
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n" + String(INDEX_HTML));
  }
  
  delay(1);
  client.stop();
}

void setup() {
  Serial.begin(115200);
  
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); 

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  
  Serial.println("\nPico W WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // NTP Time Sync for Telegram SSL
  configTime(5.5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync...");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime Synchronized!");

  secClient.setInsecure(); 
  server.begin(); 

  // Send startup message to Telegram
  bot.sendMessage(CHAT_ID, "🚀 *Raspberry Pi Pico W is Online!* Type /start to open the dashboard.", "Markdown");
}

void loop() {
  handleWebServer(); 

  // Check for Telegram Messages
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      handleTelegramMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}