#pragma once
/*************************************************
Author:zr
Date:2018-02-22
Description:Bind Connect����ӿ�
**************************************************/

#include "IMsg.hpp"

class IBind :
	public virtual IMsg
{
public:
	typedef std::vector<uint8_t> bytes;
	IBind(uint32_t cmdID) :
		log_type_(1),//��½���� 1:SP->SMG; 2:SMG->SP; 3:SMG->SMG
		log_name_(""),//��¼��
		log_pass_("")
		//��½����
	{
		this->cmdID = cmdID;
	};
	virtual bytes Binder(std::string log_id, std::string pass_word) = 0;
	virtual void recvBind(bytes &buf) = 0;
	// ȡ�û���
	virtual std::string getName() {
		return log_name_;
	};
	// ȡ����
	virtual std::string getPass()
	{
		return log_pass_;
	};
	// ȡ��¼���͡�
	virtual unsigned char getType() {
		return log_type_;
	};
	// ���õ�¼���͡�SGIP
	//1:SP->SMG; 2:SMG->SP; 3:SMG->SMG
	virtual void setType(uint8_t type) {
		log_type_ = type;
	};
	//˫��Э�̵İ汾��(��λ4bit��ʾ���汾��,��λ4bit��ʾ�ΰ汾��)
	virtual char getVersion() { return version_; };
	//ʱ���������,�ɿͻ��˲���,��ʽΪMMDDHHMMSS��������ʱ���룬10λ���ֵ����ͣ��Ҷ���
	virtual unsigned int getTimestamp() {
		return timestamp_;
	};
	virtual ~IBind(){};
protected:
	unsigned char log_type_;//��½���� 1:SP->SMG; 2:SMG->SP; 3:SMG->SMG
	std::string log_name_;//��¼��
	std::string log_pass_;//��½����
	char version_;//˫��Э�̵İ汾��(��λ4bit��ʾ���汾��,��λ4bit��ʾ�ΰ汾��)
	unsigned int timestamp_;//ʱ���������,�ɿͻ��˲���,��ʽΪMMDDHHMMSS��������ʱ���룬10λ���ֵ����ͣ��Ҷ���
};

