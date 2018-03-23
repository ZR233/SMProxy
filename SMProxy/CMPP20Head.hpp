#pragma once

/*************************************************
Author:zr
Date:2018-03-5
Description:CMPP消息头处理
**************************************************/

#include "boost/asio.hpp"
#include "IHead.hpp"
#include "exception.hpp"

//CMPP消息头处理
class CMPP20Head :
	public IHead
{
public:
	typedef std::vector<uint8_t> bytes;
	CMPP20Head()
	{
		SPID = "0";
	};
	virtual ~CMPP20Head() {};
	//制作消息头
	//@msgL	消息总长度(含消息头及消息体)
	//@CID	命令或响应类型
	//@SN	消息流水号,顺序累加,步长为1,循环使用（一对请求和应答消息的流水号必须相同）
	bytes& header(uint32_t msgL, uint32_t  CID, std::string SN) override
	{
		total_len = msgL;
		cmdID = CID;
		uint32_t sequence_id = uint32_t(stoul(SN));

		total_len = htonl(total_len);
		cmdID = htonl(cmdID);
		sequence_id = htonl(sequence_id);

		char temp[4];
		buf.clear();


		memcpy(temp, &total_len, 4);
		buf.insert(buf.end(), &temp[0], &temp[4]);

		memcpy(temp, &cmdID, 4);
		buf.insert(buf.end(), &temp[0], &temp[4]);

		memcpy(temp, &sequence_id, 4);
		buf.insert(buf.end(), &temp[0], &temp[4]);

		return buf;
	}
	// 解析消息头
	void recvHead(bytes buf)override {
		if (buf.size() < 12)
		{
			throw smproxy::exception("消息头字符串长度过短", 1);
		}

		memcpy_s(&total_len, 4, &buf[0], 4);
		memcpy_s(&cmdID, 4, &buf[4], 4);
		memcpy_s(&int_serial_numb, 4, &buf[8], 4);

		total_len = ntohl(total_len);
		cmdID = ntohl(cmdID);
		int_serial_numb = ntohl(int_serial_numb);
		serial_numb = std::to_string(int_serial_numb);
	}

};