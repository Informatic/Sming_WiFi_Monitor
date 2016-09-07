#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "proto.h"

#define LED_PIN 0 // GPIO number
#define WLAN_CHANNEL 6
#define MAC_ARGS(buf, i) buf[i+0], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5]
#define MAC_FORMAT "%02X:%02X:%02X:%02X:%02X:%02X"

uint8 sources[1000][6];

String formatMAC(uint8* hwaddr)
{
	String mac;
	for (int i = 0; i < 6; i++)
	{
		if (hwaddr[i] < 0x10) mac += "0";
		mac += String(hwaddr[i], HEX);
		if (i < 5) mac += ":";
	}
	return mac;
}
bool equalMAC(uint8* hwaddr1, uint8* hwaddr2)
{	
	for (int j = 0; j < 6; j++) {
		if (hwaddr1[j] != hwaddr2[j]) return false;
	}
	return true;
}
bool ledState = false;

static void ICACHE_FLASH_ATTR promiscCallback(uint8* buf, uint16 len)
{
	struct RxControl* control = (struct RxControl*) buf;
	struct IEEE80211_Header* header = 0;

	// Read here for more information:
	// https://espressif.com/sites/default/files/documentation/8k-esp8266_sniffer_introduction_en_v0.3.pdf
	if(len == 128) {
		// packet with header
		header = (struct IEEE80211_Header*) (buf + sizeof(struct RxControl));
	} else if(len % 10 == 0) {
		header = (struct IEEE80211_Header*) (buf + sizeof(struct RxControl));
	} else if(len == 12) {
		// No buf
		header = 0;
	} else {
		return; // Invalid length!
	}

	if (header) {
		// Ignore beacon frames
		if (header->frameControl.Type == FRAME_TYPE_MGMT && header->frameControl.Subtype == FRAME_SUBTYPE_BEACON)
			return;
		uint8* sourceMAC = 0;
		switch ((header->frameControl.ToDS<<1) | header->frameControl.FromDS) {
			case 0: sourceMAC = header->address2; break;
			case 1: sourceMAC = header->address3; break;
			case 2: sourceMAC = header->address2; break;
			case 3: sourceMAC = header->address4; break;
		}
		int i = 0;
		while (sources[i][0]|sources[i][1]|sources[i][2]|sources[i][3]|sources[i][4]|sources[i][5] || i == 1000) {
			if (equalMAC(sources[i], sourceMAC)) return;
			i++;
		}
		if (i == 1000) return;//overflow
		for (int j = 0; j < 6; j++) sources[i][j] = sourceMAC[j];
	}
	digitalWrite(LED_PIN, ledState);
	ledState = !ledState;
}
void onSerialCallback(Stream& stream, char arrivedChar, unsigned short availableCharsCount)
{
	if (arrivedChar == WLAN_CHANNEL+48) {
		for (int i = 0; i < 1000; i++ ) {
			if (sources[i][0]|sources[i][1]|sources[i][2]|sources[i][3]|sources[i][4]|sources[i][5]) {
				//Serial.println(sources[i]);
				Serial.println(formatMAC(sources[i]));
				for (int j = 0; j < 6; j++) sources[i][j] = 0;
			}
		}
		Serial.println(formatMAC(sources[999]));//its empty
	}
}
void ready()
{
	wifi_station_disconnect();
	wifi_station_set_config(NULL);
	wifi_set_opmode(1);
	Serial.println("System ready");
	
	wifi_promiscuous_enable(0);
	wifi_set_promiscuous_rx_cb(promiscCallback);
	wifi_promiscuous_enable(1);
	Serial.println("Promicuous mode started");

	wifi_set_channel(WLAN_CHANNEL);
	Serial.printf("Channel was set to %d\n\r", wifi_get_channel());
}

void init()
{
	pinMode(LED_PIN, OUTPUT);

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial
	Serial.setCallback(onSerialCallback);

	WifiStation.enable(true);
	WifiAccessPoint.enable(false);

	// Change CPU freq. to 160MHZ. Because we can
	System.setCpuFrequency(eCF_160MHz);
	Serial.print("New CPU frequency is:");
	Serial.println((int)System.getCpuFrequency());

	System.onReady(ready);	
}
