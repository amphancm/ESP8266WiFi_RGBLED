#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_GFX.h>
#include <ESP_Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const char* ssid = "Connectify-me";
const char* password = "12345678";

ESP8266WebServer server(80);

const int R = 14;
const int G = 12;
const int B = 13;

void handleRoot() {
  
  String red = server.arg(0);          // read RGB arguments
  String green = server.arg(1);
  String blue = server.arg(2);
  
  analogWrite(R, red.toInt());
  analogWrite(G, green.toInt());
  analogWrite(B, blue.toInt());

   Serial.println(red.toInt());   // for TESTING
   Serial.println(green.toInt()); // for TESTING
   Serial.println(blue.toInt());  // for TESTING

  display.clearDisplay();
  display.setTextColor(WHITE);
    
  display.setTextSize(2); 
  display.setTextColor(WHITE); 
  display.setCursor(0,0);      
  display.print("R = ");  display.println(red.toInt());
  display.setCursor(0,18);
  display.print("G = ");  display.println(green.toInt());
  display.setCursor(0,36);
  display.print("B = ");  display.println(blue.toInt()); 
  display.display();

  String webpage;     
      webpage += "<!DOCTYPE HTML>\r\n";
      webpage += "<html>\r\n";
      webpage += "<header><title>ESP8266 RGB LED</title><h1>ESP8266 RGBLED</h1></header>";
      
      webpage += "<head>";    
      webpage += "<meta name='mobile-web-app-capable' content='yes' />";
      webpage += "<meta name='viewport' content='width=device-width' />";
      webpage += "</head>";

      webpage += "<body style='margin: 0px; padding: 0px;'>";
      webpage += "<canvas id='colorspace'></canvas></body>";
     
      webpage += "<script type='text/javascript'>";
      webpage += "(function () {";
      webpage += " var canvas = document.getElementById('colorspace');";
      webpage += " var ctx = canvas.getContext('2d');";
      webpage += " function drawCanvas() {";
      webpage += " var colours = ctx.createLinearGradient(0, 0, window.innerWidth, 0);";
      webpage += " for(var i=0; i <= 360; i+=10) {";
      webpage += " colours.addColorStop(i/360, 'hsl(' + i + ', 100%, 50%)');";
      webpage += " }";
      
      webpage += " ctx.fillStyle = colours;";
      webpage += " ctx.fillRect(0, 0, window.innerWidth, window.innerHeight);";
      webpage += " var luminance = ctx.createLinearGradient(0, 0, 0, ctx.canvas.height);";
      webpage += " luminance.addColorStop(0, '#ffffff');";
      webpage += " luminance.addColorStop(0.05, '#ffffff');";
      webpage += " luminance.addColorStop(0.5, 'rgba(0,0,0,0)');";
      webpage += " luminance.addColorStop(0.95, '#000000');";
      webpage += " luminance.addColorStop(1, '#000000');";
      webpage += " ctx.fillStyle = luminance;";
      webpage += " ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);";
      webpage += " }";
      webpage += " var eventLocked = false;";
      
      webpage += " function handleEvent(clientX, clientY) {";
      webpage += " if(eventLocked) {";
      webpage += " return;";
      webpage += " }";
      
      webpage += " function colourCorrect(v) {";
      webpage += " return Math.round(1023-(v*v)/64);";
      webpage += " }";
      webpage += " var data = ctx.getImageData(clientX, clientY, 1, 1).data;";
      webpage += " var params = [";
      webpage += " 'r=' + colourCorrect(data[0]),";
      webpage += " 'g=' + colourCorrect(data[1]),";
      webpage += " 'b=' + colourCorrect(data[2])";
      webpage += " ].join('&');";
      webpage += " var req = new XMLHttpRequest();";
      webpage += " req.open('POST', '?' + params, true);";
      webpage += " req.send();";
      webpage += " eventLocked = true;";
      webpage += " req.onreadystatechange = function() {";
      webpage += " if(req.readyState == 4) {";
      webpage += " eventLocked = false;";
      webpage += " }";
      webpage += " }";
      webpage += " }";
      webpage += " canvas.addEventListener('click', function(event) {";
      webpage += " handleEvent(event.clientX, event.clientY, true);";
      webpage += " }, false);";
      webpage += " canvas.addEventListener('touchmove', function(event){";
      webpage += " handleEvent(event.touches[0].clientX, event.touches[0].clientY);";
      webpage += "}, false);";
      webpage += " function resizeCanvas() {";
      webpage += " canvas.width = window.innerWidth;";
      webpage += " canvas.height = window.innerHeight;";
      webpage += " drawCanvas();";
      webpage += " }";
      
      webpage += " window.addEventListener('resize', resizeCanvas, false);";
      webpage += " resizeCanvas();";
      webpage += " drawCanvas();";
      webpage += " document.ontouchmove = function(e) {e.preventDefault()};";
      webpage += " })();";   
      webpage += "</script><html>\r\n";

      server.send(200, "text/html", webpage);    
}


void testRGB() { // fade in and out of Red, Green, Blue
    analogWrite(R, 1023);     // R off
    analogWrite(G, 1023);     // G off
    analogWrite(B, 1023);     // B off 
    fade(R); // R
    fade(G); // G
    fade(B); // B
}

void fade(int pin) {
    for (int u = 0; u < 1024; u++) {
      analogWrite(pin,  1023 - u);
      delay(1);
    }
    for (int u = 0; u < 1024; u++) {
      analogWrite(pin, u);
      delay(1);
    }
}

void handleNotFound(){
  //digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  //digitalWrite(led, 0);
}

void setup(void){
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  analogWrite(R, 600);     // R 
  analogWrite(G, 600);     // G 
  analogWrite(B, 600);     // B 

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3D);      // initialize with the I2C addr 0x3D (for the 128x64)
  display.begin(SSD1306_SWITCHCAPVCC, 0x78>>1);     // init done
 
  display.clearDisplay();                 // Clear the buffer.
  display.setTextSize(2);
  display.setTextColor(WHITE);
  //display.setTextColor(BLACK, WHITE);   // 'inverted' text
  display.setCursor(0,0);
  display.println("  ESP8266");

  display.setTextSize(3);                                  //Size4 = 5 digit , size3 = 7 digits
  //display.setTextColor(BLACK, WHITE);   // 'inverted' text
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.println("RGB LED");
    
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.setCursor(0,52);
  display.println("Version 0.1");
 
  display.display();
  delay(2000);

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    //display.setTextColor(BLACK, WHITE); // 'inverted' text
    display.setCursor(0,0);
    display.println("Connecting");
  

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();    
  display.setTextSize(1);       display.setTextColor(WHITE);  
  display.setCursor(0,0);       display.print("SSID:"); display.println(ssid);  
  display.setTextSize(2);       display.setTextColor(WHITE); 
  display.setCursor(0,18);      display.println(WiFi.localIP());
  display.display();

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  testRGB();

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}
