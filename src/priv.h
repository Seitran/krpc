#ifndef __KRPC_PRIV_H__
#define __KRPC_PRIV_H__

#define KRPC_BUFFER_STATE_FREE		0x00000001
#define KRPC_BUFFER_STATE_PENDING	0x00000002
#define KRPC_BUFFER_STATE_COMPLETED	0x00000010

#define KRPC_TYPE_REQUEST	0
#define KRPC_TYPE_RESPONSE	1

struct buff_desc {
	u32 state;
	u32 type;
	u32 data_size;
	u8 *buffer;
};

struct krpc_msg_hdr {
	u32 channel;
	u32 type;
	u32 data_size;
	u8 data[0];
};

#endif
