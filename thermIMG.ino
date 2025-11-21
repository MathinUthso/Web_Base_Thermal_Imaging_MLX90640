#include <Wire.h>
#include <Adafruit_MLX90640.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid="UIU-STUDENT";
const char* password="12345678";


Adafruit_MLX90640 mlx;
WebServer server(80);

float frame[32 * 24];



const char MAIN_page[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Thermal Cam</title>
<style>
  body { margin:0; background:#000; }
  canvas { display:block; margin:auto; }
</style>
</head>
<body>
<canvas id="c" width="320" height="240"></canvas>

<script>
const cols = 32;
const rows = 24;
const scale = 10;  // each pixel becomes 10×10 block
const canvas = document.getElementById("c");
const ctx = canvas.getContext("2d");

function getColor(t, minT, maxT) {
  let k = (t - minT) / (maxT - maxT + 0.0001);
  k = (t - minT) / (maxT - minT);
  k = Math.max(0, Math.min(1, k));
  
  let r = k * 255;
  let g = (1 - Math.abs(k - 0.5)*2) * 255;
  let b = (1-k) * 255;
  return `rgb(${r},${g},${b})`;
}

async function updateFrame() {
  const res = await fetch("/frame");
  const txt = await res.text();
  const arr = txt.split(',').map(Number);

  const minT = Math.min(...arr);
  const maxT = Math.max(...arr);

  let i = 0;
  for(let y=0; y<rows; y++){
    for(let x=0; x<cols; x++){
      ctx.fillStyle = getColor(arr[i], minT, maxT);
      ctx.fillRect(x*scale, y*scale, scale, scale);
      i++;
    }
  }
}

setInterval(updateFrame, 100);
</script>
</body>
</html>
)====";

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleFrame() {
  mlx.getFrame(frame);

  String out = "";
  for(int i=0; i < 768; i++){
    out += String(frame[i]);
    if(i < 767) out += ",";
  }

  server.send(200, "text/plain", out);
}
void setup() {
  Serial.begin(115200);

  // I2C init
  Wire.begin(21, 22);       // I2C pins
  Wire.setClock(1000000);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
  Serial.println(WiFi.status());
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  
  if (!mlx.begin()) {
    Serial.println("MLX90640 missing!");
    while(1);
  }

  mlx.setMode(MLX90640_CHESS);
  mlx.setRefreshRate(MLX90640_8_HZ);

  
  server.on("/", handleRoot);
  server.on("/frame", handleFrame);
  server.begin();
}


void loop() {
  server.handleClient();
}

