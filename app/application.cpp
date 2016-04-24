#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include "proto.h"

#define LED_PIN 2 // GPIO number

#define MAC_ARGS(buf, i) buf[i+0], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5]
#define MAC_FORMAT "%02X:%02X:%02X:%02X:%02X:%02X"

//int counter = 0;
Timer channelHopTimer;

void channelHop()
{
    // 1 - 13 channel hopping
    uint8 new_channel = wifi_get_channel() % 12 + 1;
    Serial.printf("** Hopping to %d **\r\n", new_channel);
    wifi_set_channel(new_channel);
}

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

bool ledState = false;

static void ICACHE_FLASH_ATTR promiscCallback(uint8* buf, uint16 len)
{
	struct RxControl* control = 0;

	if(len == 128) {
		// packet with header
		control = (struct RxControl*) buf;
		buf += sizeof(struct RxControl);
	}

	struct IEEE80211_Header* header = (struct IEEE80211_Header*) buf;

	// Ignore beacon frames
	if (header->frameControl.Type == FRAME_TYPE_MGMT && header->frameControl.Subtype == FRAME_SUBTYPE_BEACON)
		return;

	Serial.printf("len: %4d, chan: %2d, proto: %d, type: %2d-%2d ", len, wifi_get_channel(), header->frameControl.Protocol, header->frameControl.Type, header->frameControl.Subtype);

	Serial.print(formatMAC(header->address1) + " ");
	Serial.print(formatMAC(header->address2) + " ");
	Serial.print(formatMAC(header->address3) + " ");
	Serial.print(formatMAC(header->address4));

	// Display rxcontrol at the end of the line for better formatting
	if (control)
	{
		Serial.printf(" [RxControl rssi: %d legacy_length: %d chan: %d]", control->rssi, control->legacy_length, control->channel);
	}

	Serial.println();

	digitalWrite(LED_PIN, ledState);
	ledState = !ledState;
}

void ready()
{
	wifi_station_disconnect();
	wifi_station_set_config(NULL);
	wifi_set_opmode(1);
	Serial.println("System ready");

	wifi_promiscuous_enable(1);
    wifi_set_promiscuous_rx_cb(promiscCallback);
	Serial.println("Promicuous mode started");

	channelHopTimer.initializeMs(5000, channelHop).start();
	Serial.println("Channel hopper started");
	wifi_set_channel(1);
}

void init()
{
	pinMode(LED_PIN, OUTPUT);

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	WifiStation.enable(false);
	WifiAccessPoint.enable(false);

	// Change CPU freq. to 160MHZ. Because we can
	System.setCpuFrequency(eCF_160MHz);
	Serial.print("New CPU frequency is:");
	Serial.println((int)System.getCpuFrequency());

	System.onReady(ready);
}
