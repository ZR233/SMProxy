#pragma once
/*************************************************
Author:zr
Date:2018-03-5
Description:��Ϣ��ӿ�
**************************************************/



#include "IMsg.hpp"
#include "IDeliver.hpp"


/**
* \brief submit �ӿ�
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
	* \brief  ����submit buf
	* \param userNum �绰����vector
	* \param msg ��������
	* \return ���ͻ����ַ���
	*/
	virtual bytes submit(std::vector<std::string> &userNum, std::string &msg) = 0;
	virtual	bytes submit(std::vector<std::string> &userNum, uint8_t msg_fmt, std::vector<uint8_t> &msg)
	{
		bytes a;
		return a;
	};
	// ����submit
	virtual void recvSubmit(bytes &buf) = 0;

	virtual std::vector<std::string>* getPhNums() { return &num_; };
	// ȡ����Ϣ����
	virtual int getTextL() { return message_length_; };
	// ȡ����Ϣ����
	virtual std::string* getText() { return &message_content_; };
	// ȡ�ֻ�������
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
	uint8_t pk_total_;					//	��ͬMsg_Id����Ϣ������
	uint8_t pk_number_;					//	��ͬMsg_Id����Ϣ���
	uint8_t msg_level_;					//	��Ϣ����
	std::string service_id_;				//	ҵ������
	uint8_t fee_usertype_;				//	�Ʒ��û������ֶ�
	std::string fee_number_;			//	���Ʒ��û��ĺ���
	uint8_t tp_pid_;					//	TP-Protocol-Identifier
	uint8_t tp_udhi_;					//	TP-User-Data-Header-Indicator
	uint8_t msg_fmt_;					//	��Ϣ��ʽ��8��UCS2���� 15:��GB����
										//std::string msg_src_;				//	��Ϣ������Դ(SP_Id)
	std::string fee_type_;				//	�ʷ����
	std::string fee_code_;				//	�ʷѴ��루�Է�Ϊ��λ��
	std::string valid_time_;				//	�����Ч�ڣ��μ�SMPP3.3
	std::string at_time_;				//	��ʱ����ʱ�䣬�μ�SMPP3.3 
	uint8_t dest_total_;					//	������Ϣ���û�����(===1)

	std::string sp_number_;				//SP����� 
	std::string charge_number_;			//���Ѻ��룬�ֻ�����ǰ�ӡ�86�������־�����ҽ���Ⱥ���Ҷ��û��շ�ʱΪ�գ�
										//���Ϊ�գ����������Ϣ�����ķ�����UserNumber������û�֧����
										//���Ϊȫ���ַ�����000000000000000000000������ʾ��������Ϣ�����ķ�����SP֧����*/
	std::string service_type_;//ҵ�����ͣ������֡���ĸ�ͷ��ŵ���ϡ�
	std::string fee_value_;
	std::string given_value_;
	/**
	* \brief ���շѱ�־��0��Ӧ�գ�1��ʵ��
	*/
	uint8_t agent_flag_;
	/**
	* \brief ����MT��Ϣ��ԭ��
	* 0-MO�㲥����ĵ�һ��MT��Ϣ��
	* 1-MO�㲥����ķǵ�һ��MT��Ϣ��
	* 2-��MO�㲥�����MT��Ϣ��
	* 3-ϵͳ���������MT��Ϣ��
	*/
	uint8_t morelateto_MT_flag_;
	uint8_t report_flag_; //	�Ƿ�Ҫ�󷵻�״̬ȷ�ϱ���
						  //char TP_pid;
						  //char TP_udhi;
						  //char message_coding;
	uint8_t message_type_;
	//���ų���
	uint32_t message_length_;
	//��������
	std::string message_content_;
	//һ���������з�����
	int same_msg_total_;
	//�������зֿ��ĵڼ���
	int same_msg_num_;
	//��������Ƭid
	uint32_t long_msg_id_;
	std::vector<std::string> num_;//�ֻ���vector
};

