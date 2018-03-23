#pragma once
#include <string>
#include <vector>

/*************************************************
Author:zr
Date:2018-03-5
Description:head 接口
**************************************************/

//head 接口
class IHead
{
	typedef std::vector<uint8_t> bytes;
public:
	IHead() :
		SPID(""),
		SN2(0),
		cmdID(0),
		serial_numb(""),
		total_len(0)
	{
		buf.clear();
	}
	//SGIP1.2 SN1
	virtual void setSPID(std::string SPID) { this->SPID = SPID; };
	//SGIP1.2 SN2
	virtual void setSN2(uint32_t SN2) { this->SN2 = SN2; };
	//生成下一个流水号
	virtual std::string createSerualNum(uint32_t& serial_numb) {
		if (serial_numb >99999)
		{
			serial_numb = 0;
		}
		++serial_numb;
		return std::to_string(serial_numb);
	};
	virtual bytes& header(uint32_t msg_len, uint32_t cmdID, std::string serial_numb) = 0;
	virtual void recvHead(bytes) = 0;
	// 消息长度
	virtual uint32_t getMsgLen() { return total_len; };
	// 指令ID
	virtual uint32_t getCmdID() { return cmdID; };
	// SN1
	virtual std::string getSPID() { return SPID; };
	// SN2
	virtual uint32_t getSN2() { return SN2; };
	// 流水号
	virtual std::string getSerialNumb() { return serial_numb; };
	//取缓存
	virtual bytes& getBuf() { return buf; };
	// 取纯数字流水号
	virtual uint32_t getIntSerialNumb() { return int_serial_numb; };
	virtual ~IHead() {};
protected:
	std::string SPID;
	uint32_t SN2;
	uint32_t cmdID;
	uint32_t int_serial_numb;
	std::string serial_numb;
	uint32_t total_len;
	bytes buf;
};

