#pragma once
/*************************************************
Author:zr
Date:2018-03-5
Description:消息体接口
**************************************************/



#include "IMsg.hpp"
#include "IDeliver.hpp"


/**
* \brief submit 接口
*/
class ISubmit :
	public virtual IMsg
{
public:
	typedef std::vector<uint8_t> bytes;
	ISubmit(uint32_t cmdID)
		: pk_total_(0), pk_number_(0), msg_level_(0), fee_usertype_(0), tp_pid_(0), tp_udhi_(0), msg_fmt_(0),
		dest_total_(0), agent_flag_(0), morelateto_MT_flag_(2), report_flag_(1), message_type_(0), message_length_(0),
		same_msg_total_(0),
		same_msg_num_(0),
		long_msg_id_(0)
	{
		this->cmdID = cmdID;
		msg_id = "";
		msg_id_this = "";
		buf_.clear();
	}

	virtual	~ISubmit() = default;


	/**
	* \brief  生成submit buf
	* \param userNum 电话号码vector
	* \param msg 短信内容
	* \return 发送缓存字符串
	*/
	virtual bytes submit(std::vector<std::string> &userNum, std::string &msg) = 0;
	virtual	bytes submit(std::vector<std::string> &userNum, uint8_t msg_fmt, std::vector<uint8_t> &msg)
	{
		bytes a;
		return a;
	};
	// 接收submit
	virtual void recvSubmit(bytes &buf) = 0;

	virtual std::vector<std::string>* getPhNums() { return &num_; };
	// 取短消息长度
	virtual int getTextL() { return message_length_; };
	// 取短消息内容
	virtual std::string* getText() { return &message_content_; };
	// 取手机号数量
	virtual int getUserCount() { return int(dest_total_); };
	virtual void setTP_pid(uint8_t TP_pid) { tp_pid_ = TP_pid; };
	int getTP_udhi() { return tp_udhi_; };
	virtual void setTP_udhi(uint8_t TP_udhi) { tp_udhi_ = TP_udhi; };
	virtual std::vector<uint8_t> stringToUCS2_bytes(std::string src) {
		IDeliver de(1);
		return de.stringToUCS2_bytes(src);
	};
	virtual int getSame_msg_total() { return same_msg_total_; };
	virtual int getSame_msg_num() { return same_msg_num_; };
	virtual uint32_t getLong_msg_id() { return long_msg_id_; };
	int getPk_total() { return  pk_total_; };
	virtual void setPk_total(uint8_t  pk_total) { pk_total_ = pk_total; };
	virtual int getPk_number() { return  pk_number_; };
	virtual void setPk_number(uint8_t pk_number) { pk_number_ = pk_number; };
protected:
	uint8_t pk_total_;					//	相同Msg_Id的信息总条数
	uint8_t pk_number_;					//	相同Msg_Id的信息序号
	uint8_t msg_level_;					//	信息级别
	std::string service_id_;				//	业务类型
	uint8_t fee_usertype_;				//	计费用户类型字段
	std::string fee_number_;			//	被计费用户的号码
	uint8_t tp_pid_;					//	TP-Protocol-Identifier
	uint8_t tp_udhi_;					//	TP-User-Data-Header-Indicator
	uint8_t msg_fmt_;					//	信息格式，8：UCS2编码 15:含GB汉字
										//std::string msg_src_;				//	信息内容来源(SP_Id)
	std::string fee_type_;				//	资费类别
	std::string fee_code_;				//	资费代码（以分为单位）
	std::string valid_time_;				//	存活有效期，参见SMPP3.3
	std::string at_time_;				//	定时发送时间，参见SMPP3.3 
	uint8_t dest_total_;					//	接收信息的用户数量(===1)

	std::string sp_number_;				//SP接入号 
	std::string charge_number_;			//付费号码，手机号码前加“86”国别标志；当且仅当群发且对用户收费时为空；
										//如果为空，则该条短消息产生的费用由UserNumber代表的用户支付；
										//如果为全零字符串“000000000000000000000”，表示该条短消息产生的费用由SP支付。*/
	std::string service_type_;//业务类型，是数字、字母和符号的组合。
	std::string fee_value_;
	std::string given_value_;
	/**
	* \brief 代收费标志，0：应收；1：实收
	*/
	uint8_t agent_flag_;
	/**
	* \brief 引起MT消息的原因
	* 0-MO点播引起的第一条MT消息；
	* 1-MO点播引起的非第一条MT消息；
	* 2-非MO点播引起的MT消息；
	* 3-系统反馈引起的MT消息。
	*/
	uint8_t morelateto_MT_flag_;
	uint8_t report_flag_; //	是否要求返回状态确认报告
						  //char TP_pid;
						  //char TP_udhi;
						  //char message_coding;
	uint8_t message_type_;
	//短信长度
	uint32_t message_length_;
	//短信内容
	std::string message_content_;
	//一条长短信切分数量
	int same_msg_total_;
	//长短信切分开的第几条
	int same_msg_num_;
	//长短信切片id
	uint32_t long_msg_id_;
	std::vector<std::string> num_;//手机号vector
};

