#ifndef PROTO_H

//
// Rewritten from some random example, not sure...
//

#define FRAME_TYPE_MGMT			0
#define FRAME_TYPE_CTRL			1
#define FRAME_TYPE_DATA			2

#define FRAME_SUBTYPE_ASSOC_REQ		0
#define FRAME_SUBTYPE_ASSOC_RESP	1
#define FRAME_SUBTYPE_REASSOC_REQ	2
#define FRAME_SUBTYPE_REASSOC_RESP	3
#define FRAME_SUBTYPE_PROBE_REQ		4
#define FRAME_SUBTYPE_PROBE_RESP	5
#define FRAME_SUBTYPE_BEACON		8
#define FRAME_SUBTYPE_ATIM			9
#define FRAME_SUBTYPE_DISASSOC		10
#define FRAME_SUBTYPE_AUTH			11
#define FRAME_SUBTYPE_DEAUTH		12

struct RxControl {
    signed rssi:8;
    unsigned rate:4;
    unsigned is_group:1;
    unsigned:1;
    unsigned sig_mode:2;
    unsigned legacy_length:12;
    unsigned damatch0:1;
    unsigned damatch1:1;
    unsigned bssidmatch0:1;
    unsigned bssidmatch1:1;
    unsigned MCS:7;
    unsigned CWB:1;
    unsigned HT_length:16;
    unsigned Smoothing:1;
    unsigned Not_Sounding:1;
    unsigned:1;
    unsigned Aggregation:1;
    unsigned STBC:2;
    unsigned FEC_CODING:1;
    unsigned SGI:1;
    unsigned rxend_state:8;
    unsigned ampdu_cnt:8;
    unsigned channel:4;
    unsigned:12;
};

struct IEEE80211_Header
{
	struct {
		//buf[0]
		u8 Protocol:2;
		u8 Type:2;
		u8 Subtype:4;
		//buf[1]
		u8 ToDS:1;
		u8 FromDS:1;
		u8 MoreFlag:1;
		u8 Retry:1;
		u8 PwrMgmt:1;
		u8 MoreData:1;
		u8 Protectedframe:1;
		u8 Order:1;
	} frameControl;

	u16 duration;
	u8 address1[6];
	u8 address2[6];
	u8 address3[6];

	u8 fragmentNumber:4;
	u16 sequenceNumber:12;

	u8 address4[6];
};

#define PROTO_H
#endif
