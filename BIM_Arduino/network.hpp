class Network {
  private:
    void _connecting(uint8_t num);

  public:
    bool isConnected(void);
    void connect(void);
    void scan(void);
    void runAccessPoint(void);
};

/**
 * Checking for internet access
 */
bool Network::isConnected(void) {
  if(WiFi.status() == WL_CONNECTED and WiFi.localIP().toString() != "0.0.0.0") return true;
  return false;
}

/**
 * Connect to WiFi network
 */
void Network::connect(void) {
  Serial.println(SEPARATOR);
    Serial.println("Connecting to WiFi...");
    Serial.println("Known networks:");
    for(unsigned int i=0; i<NETWORKS; i++) {
        if(String(config.network_ssid(i)) != "") {
            Serial.printf("\tNet%d: %s\r\n", i, config.network_ssid(i));
        }
    }
    bool anySSID = false;
    for(unsigned int i=0; i<NETWORKS; i++) {
        if(String(config.network_ssid(i)) != "") anySSID = true;
    }
    if(anySSID) {
        if(!global.apMode) WiFi.disconnect();
        WiFi.hostname("BIM32");
        if(WiFi.status() != WL_CONNECTED) {
            unsigned int numberOfNetworks = WiFi.scanNetworks();
            int knownNetwork = -1;
            for(int i=0; i<numberOfNetworks; i++) {
                for(int k=0; k<NETWORKS; k++) {
                    if(WiFi.SSID(i) == config.network_ssid(k)) {
                        knownNetwork = k;
                        break;
                    }
                }
            }
            if(knownNetwork >= 0) {
                Serial.print("Connecting to "); Serial.println(config.network_ssid(knownNetwork));
                _connecting(knownNetwork);
            }
            else {
                Serial.println("No known networks found");
                Serial.println("trying to connect to network 1 in case it's a hidden network");
                _connecting(0); 
            }
        }
        if(WiFi.status() == WL_CONNECTED) {
            WiFi.setAutoConnect(true);
            WiFi.setAutoReconnect(true);
            WiFi.persistent(true); 
            IPAddress ip;
            IPAddress subnet;
            IPAddress gateway;
            IPAddress dns1;
            IPAddress dns2;
            if(config.network_type()) {
                Serial.println("Configure static IP address");
                if(ip.fromString(config.network_ip()) and 
                    gateway.fromString(config.network_gw()) and subnet.fromString(config.network_mask()) and
                    dns1.fromString(config.network_dns1()) and dns2.fromString(config.network_dns2())
                ) 
                    WiFi.config(ip, gateway, subnet, dns1, dns2);
                else Serial.println("Error, switching back to dynamic IP");
            }
            Serial.print("IP address: "); Serial.println(WiFi.localIP().toString());
        }
    }
    else {
        Serial.println("No WiFi networks configured");
        runAccessPoint();
    }
}

void Network::_connecting(uint8_t num) {
    WiFi.begin(config.network_ssid(num), config.network_pass(num));
    unsigned int attempts = 0;
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if(++attempts > 50) break;
    }
    if(WiFi.status() == WL_CONNECTED) Serial.println(" connected");
    else Serial.println(" failed");
}

/**
 * Scan available networks
 */
void Network::scan(void) {
  global.networks_update = millis();
  global.nets = WiFi.scanNetworks();
  
  if(global.nets > 29) global.nets = 29;
  
  for(unsigned int i=0; i<global.nets; i++) {
    WiFi.SSID(i).toCharArray(global.ssids[i], sizeof(global.ssids[i]));
    global.rssis[i] = abs(WiFi.RSSI(i));
  }
}

/**
 * Start Access point
 */
void Network::runAccessPoint(void) {
  if(!global.apMode) {
    global.apMode = true;
    Serial.println();
    Serial.println(SEPARATOR);
    Serial.println("Start Access Point mode");
    Serial.print("Network: "); Serial.println(config.accessPoint_ssid());
    Serial.print("Password: "); Serial.println(config.accessPoint_pass());
    Serial.println("IP address: 192.168.4.1");
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.softAP(config.accessPoint_ssid(), config.accessPoint_pass());
    Serial.println(SEPARATOR);
    Serial.println();
  }
}
