#include "WiFi.h" // ESP32 WiFi include
#include "WiFiConfig.h" // My WiFi configuration.
// https://www.megunolink.com/articles/wireless/how-do-i-connect-to-a-wireless-network-with-the-esp32/

WiFiServer server(80);
 
void ConnectToWiFi()
{
 
  WiFi.begin(SSID, WiFiPassword);
  Serial.print("Connecting to "); Serial.println(SSID);
 
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
 
    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }
 
  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
  server.begin();
}

void webpage(const float tempPec, const float tempBoi, const float tempRet){
  WiFiClient client = server.available();   // Listen for incoming clients
  String header; // header ki ga prejmemo, trenutno neuporabn

  if (client) {                             // If a new client connects,
    //Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html><head><meta charset='utf-8'><title>Valilnica</title></head>");
            client.println("<body onload='setTimeout(\"location.reload(true);\", 1000);' style='margin: 30px;'>");
            
            client.println("<h1>Temparatura v peci: " + String(tempPec) + "°C</h1><br>");
            client.println("<h1>Temparatura v bojlerju: " + String(tempBoi) + "°C</h1><br>");
            client.println("<h1>Povratna temparatura: " + String(tempRet) + "°C</h1><br>");

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  }
