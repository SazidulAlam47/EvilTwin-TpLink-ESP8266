# EvilTwin-TpLink-ESP8266

A custom ESP8266-based project that mimics a TP-Link Wi-Fi Router access point to demonstrate and study the "Evil Twin" attack scenario. This project is for **educational purposes only** and should not be used maliciously.

## ⚠️ Disclaimer

This project is intended for **ethical hacking and security research**. Please use responsibly and only on networks you own or have explicit permission to test. The creator takes no responsibility for misuse of this code.

## Features

- **Fake Access Point**: Creates a Wi-Fi access point that mimics a legitimate TP-Link router.
- **Captive Portal**: Redirects connected devices to a custom web page for credential capture or further testing.
- **Customizable Settings**: Adjust SSID, web page content, and other configurations.
- **ESP8266-based**: Lightweight and easily deployable on ESP8266 hardware.

## Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) installed.
- ESP8266 board installed in Arduino IDE ([setup guide](https://arduino-esp8266.readthedocs.io/en/latest/installing.html)).
- Basic understanding of Wi-Fi networks and ethical hacking.

### Installation

1. Clone this repository:
   ```bash
   git clone https://github.com/your-username/EvilTwin-TpLink-ESP8266.git
   ```
2. Open the `.ino` file in Arduino IDE.
3. Install the following libraries via the Arduino Library Manager:
- `ESP8266WiFi`
- `DNSServer`
- `ESP8266WebServer`
4. Modify the SSID and webpage content in the code to fit your use case.
5. Compile and upload the code to your ESP8266.


### Usage
1. Power on the ESP8266 module.
2. It will create a Wi-Fi access point named TP-Link_XXXX (or the SSID you configured).
3. Connect to the fake network with a device.
4. Open any webpage to trigger the captive portal, or navigate to the IP address 192.168.4.1.
5. Use the web interface for further testing or demonstration.

### Code Overview
- Access Point Setup: The ESP8266 is configured to mimic a TP-Link router.
- Web Server: Hosts a customizable captive portal for connected devices.
- Security Considerations: Intended for controlled and ethical use only.

### Future Improvements
- Add more TP-Link-specific emulation features.
- Implement logging for captured data (e.g., credentials).
- Support WPA2-Enterprise simulation.

### ⚠️ Legal Notice
Unauthorized use of this code is illegal. Always obtain proper permissions before conducting any testing. This project is designed to help understand and protect against Wi-Fi "Evil Twin" attacks.