#ifndef _H_NET_PROTOCOL_DEFINE_
#define _H_NET_PROTOCOL_DEFINE_
#include <stdint.h>
#include "Buffer.h"
#include "zlib.h"
#include "login.pb-c.h"

/**

+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|     Int32       | Int32  |   char*            |  Int32 |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|tag(10b)+len(22b)|  sid   |  content(x)        |checksum|
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

*/


#define PACKET_TAG_AND_SIZE_LEN sizeof(int)
#define PACKET_SID_LEN sizeof(int)
#define PACKET_CHECK_SUM_LEN sizeof(int)


#define PROTO_PACK_CASE(name1,name2) \
case PROTO_TYPE_##name1: \
do  \
{   \
	int packed_dec_size = name2##__get_packed_size((name1*)base_msg);  \
	int total_len = packed_dec_size + PACKET_TAG_AND_SIZE_LEN + PACKET_CHECK_SUM_LEN + PACKET_SID_LEN; \
	uint8_t *total_buf = (uint8_t*)malloc(total_len); \
	uint8_t *content_buf_offset = total_buf + PACKET_TAG_AND_SIZE_LEN + PACKET_SID_LEN; \
	int packed_real_size = name2##__pack((name1*)base_msg, content_buf_offset); \
	if (packed_real_size == packed_dec_size){\
		*p_size = total_len; \
		return total_buf; \
	}else{\
		free(total_buf); \
		return 0; \
	}\
} while (0); \
break;

#define PROTO_UNPACK_CASE(name1,name2) \
case PROTO_TYPE_##name1: \
	do  \
	{   \
		ProtobufCMessage*base_msg = (ProtobufCMessage*)name2##__unpack(0, data_len, buffer); \
		return base_msg; \
	} while (0); \
	break;

#define PROTOBUF_PACKAGE_ID_MASK 10


extern "C" {
	struct Arc4;
	unsigned char*RSAEncrypt(unsigned char*input, int inLen, int *outLen);
	struct Arc4*InitRC4(unsigned char*keys, int keylen);
	int RC4Encrypt(struct Arc4*key, unsigned char*input, int inLen);
	char*getEncryptedPasswordHex(char*password);
	char*getStringFromEncryptedHex(char*hex);
}
;

const static int kHeaderLen = sizeof(int32_t);
const static int kMinMessageLen = 2 * kHeaderLen; //packageLen + checkSum
const static int kMaxMessageLen = 64 * 1024 * 1024; //
class ProtocolDefine
{
public:
	static void WritePacket(int type, uint8_t* msgBuf, unsigned int msgBufSize, bool doEnc, struct Arc4* encKey)
	{
		unsigned int packet_type_uint = type;
		unsigned int totalBufSize = msgBufSize;

		unsigned int packet_size_uint = totalBufSize;

		//combine msg type + packet size in only 4bytes
		packet_type_uint <<= (32 - PROTOBUF_PACKAGE_ID_MASK);
		packet_size_uint <<= PROTOBUF_PACKAGE_ID_MASK;
		packet_size_uint >>= PROTOBUF_PACKAGE_ID_MASK;
		packet_size_uint |= packet_type_uint;

		//write size into buffer
		memcpy(msgBuf, &packet_size_uint, PACKET_TAG_AND_SIZE_LEN);

		//write serial id
		int sid = 0x7f;
		memcpy(msgBuf + PACKET_TAG_AND_SIZE_LEN, &sid, sizeof(int));

		//do checksum only for msg content without last 4bytes
		unsigned int checkSum = ::adler32(1, msgBuf, (totalBufSize - PACKET_TAG_AND_SIZE_LEN));

		unsigned int checksumOffset = totalBufSize - PACKET_CHECK_SUM_LEN;

		//write checksum
		memcpy(msgBuf + checksumOffset, &checkSum, PACKET_CHECK_SUM_LEN);

		//rc4 encryption
		if (doEnc)
		{
			RC4Encrypt(encKey, msgBuf, totalBufSize);
		}
	}

	static bool ReadPacket(unsigned char* msgBuf, unsigned char* newRecv, unsigned int msgBufSize, size_t newRecvSz,
		int*rDataLen, unsigned int* rMsgType, bool doDec, struct Arc4* decKey)
	{
		if (doDec)
		{
			RC4Encrypt(decKey, newRecv, newRecvSz);
		}

		*rDataLen = -1;

		unsigned int totalPacketLen;
		memcpy(&totalPacketLen, msgBuf, PACKET_TAG_AND_SIZE_LEN);

		unsigned int msg_type = totalPacketLen >> (32 - PROTOBUF_PACKAGE_ID_MASK);
		totalPacketLen <<= PROTOBUF_PACKAGE_ID_MASK;
		totalPacketLen >>= PROTOBUF_PACKAGE_ID_MASK;

		*rMsgType = msg_type;

		if (totalPacketLen > kMaxMessageLen || totalPacketLen < kMinMessageLen)
		{
			//read error
		}
		else if(msgBufSize >= totalPacketLen) 
		{
			//buff = buff + kHeaderLen;
			bool succ = false;
			unsigned int expectedCheckSum;// = get_uint_from_buffer(buff + package_len - sizeof(int));
			unsigned int checksumOffset = totalPacketLen - PACKET_CHECK_SUM_LEN;
			memcpy(&expectedCheckSum, msgBuf + checksumOffset, PACKET_CHECK_SUM_LEN);

			unsigned int checkSum = ::adler32(1, (const unsigned char*)msgBuf, (totalPacketLen - sizeof(int)));
			if (checkSum == expectedCheckSum) 
			{
				int32_t dataLen = totalPacketLen - PACKET_CHECK_SUM_LEN - PACKET_TAG_AND_SIZE_LEN - PACKET_SID_LEN;
				*rDataLen = dataLen;
			}
			return true;
		}
		return false;
	}

public:
	ProtocolDefine()
	{}
	~ProtocolDefine()
	{}
};


#endif