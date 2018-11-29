
#include <ESP8266WiFi.h>

const char* ssid = "YourWifiSSID";
const char* password = "YourWifiPassword";

const int redPin = 4; //  ~D2
const int greenPin = 12; // ~D6
const int bluePin = 14; // ~D5


int WiFiStrength = 0;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);



  analogWrite(redPin, 280);
  analogWrite(greenPin, 300);
  analogWrite(bluePin, 300);



  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);


  WiFi.config(IPAddress(192, 168, 1, 221), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

double analogValue = 0.0;
double analogVolts = 0.0;
unsigned long timeHolder = 0;


void loop() {

  WiFiStrength = WiFi.RSSI(); // get dBm from the ESP8266
  analogValue = analogRead(A0); // read the analog signal





  analogVolts = (analogValue * 3.08) / 1024;


  int chartValue = (analogValue * 100) / 400;

  chartValue = 100 - chartValue;



  if (millis() - 15000 > timeHolder)
  {
    timeHolder = millis();



    if (chartValue <= 25) {  // 0-25 is red "dry"

      analogWrite(redPin, 1000);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);

    } else if (chartValue > 25 && chartValue <= 75) // 26-75 is green
    {

      analogWrite(redPin, 0);
      analogWrite(greenPin, 1000);
      analogWrite(bluePin, 0);

    }
    else if (chartValue > 75 ) // 76-100 is blue
    {

      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 1000);

    }

    delay(1000); // this is the duration the LED will stay ON

    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);

  }

  // Serial data
  Serial.print("Analog raw: ");
  Serial.println(analogValue);
  Serial.print("Analog V: ");
  Serial.println(analogVolts);
  Serial.print("ChartValue: ");
  Serial.println(chartValue);
  Serial.print("TimeHolder: ");
  Serial.println(timeHolder);
  Serial.print("millis(): ");
  Serial.println(millis());
  Serial.print("WiFi Strength: ");
  Serial.print(WiFiStrength); Serial.println("dBm");
  Serial.println(" ");
  delay(1000); // slows amount of data sent via serial


  WiFiClient client = server.available();
  if (!client) {
    return;
  }


  Serial.println("new client");

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();


  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");

  client.println("<html>");
  client.println(" <head>");
  client.println("<meta http-equiv=\"refresh\" content=\"60\">");
  client.println(" <script type=\"text/javascript\" src=\"https://www.gstatic.com/charts/loader.js\"></script>");
  client.println("  <script type=\"text/javascript\">");
  client.println("    google.charts.load('current', {'packages':['gauge']});");
  client.println("    google.charts.setOnLoadCallback(drawChart);");
  client.println("   function drawChart() {");

  client.println("      var data = google.visualization.arrayToDataTable([ ");
  client.println("        ['Label', 'Value'], ");
  client.print("        ['Moisture',  ");
  client.print(chartValue);
  client.println(" ], ");
  client.println("       ]); ");

  client.println("    var options = {");
  client.println("      width: 400, height: 120,");
  client.println("      redFrom: 0, redTo: 25,");
  client.println("      yellowFrom: 25, yellowTo: 75,");
  client.println("      greenFrom: 75, greenTo: 100,");
  client.println("       minorTicks: 5");
  client.println("    };");

  client.println("   var chart = new google.visualization.Gauge(document.getElementById('chart_div'));");

  client.println("  chart.draw(data, options);");

  client.println("  setInterval(function() {");
  client.print("  data.setValue(0, 1, ");
  client.print(chartValue);
  client.println("    );");
  client.println("    chart.draw(data, options);");
  client.println("    }, 13000);");


  client.println("  }");
  client.println(" </script>");

  client.println("  </head>");
  client.println("  <body>");

  client.print("<h1 style=\"size:12px;\">ESP8266 Soil Moisture</h1>");


  client.println("<table><tr><td>");

  client.print("WiFi Signal Strength: ");
  client.println(WiFiStrength);
  client.println("dBm<br>");
  client.print("Analog Raw: ");
  client.println(analogValue);
  client.print("<br>Analog Volts: ");
  client.println(analogVolts);
  client.println("<br><a href=\"/REFRESH\"\"><button>Refresh</button></a>");

  client.println("</td><td>");

  client.println("<div id=\"chart_div\" style=\"width: 300px; height: 120px;\"></div>");
  client.println("</td></tr></table>");

  client.println("<body>");
  client.println("</html>");
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");


}
