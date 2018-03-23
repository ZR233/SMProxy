#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "IRespAll.hpp"
#include "IReport.hpp"
#include "ISubmit.hpp"
#include "IDeliver.hpp"


#include "CMPP20Connect.hpp"
#include "CMPP20Submit.hpp"
#include "CMPP20Deliver.hpp"


#include "SGIP12Deliver.hpp"
#include "SGIP12Report.hpp"
#include "SGIP12Submit.hpp"
#include "SGIP12Bind.hpp"
#include "SGIP12Resp.hpp"
#include "CMPP20Resp.hpp"
/*************************************************
Author:zr
Date:2018-01-18
Description:���� �ַ���
**************************************************/





//���� �ַ���
class CResolve
{
public:
	CResolve() : result(0), cmd_id(), time_stamp(0), pk_total(0), pk_number(0), TP_udhi(0), same_msg_total(0),
		same_msg_num(0), long_msg_id(0)
	{
		SN1 = 0;
		SN2 = 0;
		SN3 = 0;
		resp_ = 5;
		total_len = 0;
	}
	~CResolve(){}
	enum Eprotocol
	{
		SGIP12 = 1,
		CMPP20 = 2
	};
	enum Ecommand
	{
		Bind = 1,BindResp = 0x80000001,
		UnBind = 2,UnBindResp = 0x80000002,
		Submit = 3,SubmitResp = 0x80000003,
		Deliver = 4,DeliverResp = 0x80000004,
		Report = 5,ReportResp = 0x80000005,
		ActiveTest = 8, ActiveTestResp = 0x80000008
	};
	//mode 1:SGIP1.2  2:CMPP2.0
	void setProtocol(Eprotocol protocol)
	{
		protocol_ = protocol;
		switch (protocol_)
		{
		case SGIP12:
			ch = &sgip12_hd;
			cb = &sgip12_cb;
			cs = &sgip12_cs;
			cr = &sgip12_cr;
			dlv = &sgip12_dlv;
			rp = &sgip12_rp;
			break;
		case CMPP20:
			ch = &cmpp20_ch;
			cb = &cmpp20_cb;
			cs = &cmpp20_cs;
			cr = &cmpp20_cr;
			dlv = &cmpp20_dlv;
			rp = &cmpp20_dlv;
			break;
		default:
			break;
		}
	}
	void recvHead(std::vector<uint8_t> &buf)
	{
		ch->recvHead(buf);
		switch (protocol_)
		{
		case SGIP12:
			switch (ch->getCmdID())
			{
			case 1:
				cmd_id = Bind;
				break;
			case 0x80000001:
				cmd_id = BindResp;
				break;
			case 2:
				cmd_id = UnBind;
				break;
			case 0x80000002:
				cmd_id = UnBindResp;
				break;
			case 3:
				cmd_id = Submit;
				break;
			case 0x80000003:
				cmd_id = SubmitResp;
				break;
			case 4:
				cmd_id = Deliver;
				break;
			case 0x80000004:
				cmd_id = DeliverResp;
				break;
			case 5:
				cmd_id = Report;
				break;
			case 0x80000005:
				cmd_id = ReportResp;
				break;
			default:
				throw smproxy::exception("[CResovle.cpp]����ָ��ţ�" + std::to_string(ch->getCmdID()), 7);
			}
			break;
		case CMPP20:
			switch (ch->getCmdID())
			{
			case 1:
				cmd_id = Bind;
				break;
			case 0x80000001:
				cmd_id = BindResp;
				break;
			case 2:
				cmd_id = UnBind;
				break;
			case 0x80000002:
				cmd_id = UnBindResp;
				break;
			case 4:
				cmd_id = Submit;
				break;
			case 0x80000004:
				cmd_id = SubmitResp;
				break;
			case 5:
				cmd_id = Deliver;
				break;
			case 0x80000005:
				cmd_id = DeliverResp;
				break;
			case 8:
				cmd_id = ActiveTest;
				break;
			case 0x80000008:
				cmd_id = ActiveTestResp;
				break;
			default:
				throw smproxy::exception("[CResovle.cpp]����ָ��ţ�" + std::to_string(ch->getCmdID()), 2);
			}
			break;
		}
		SN1 = std::stoll(ch->getSPID());
		SN2 = ch->getSN2();
		SN3 = ch->getIntSerialNumb();
		serial_numb = ch->getSerialNumb();
	}
	void recvBody(std::vector<uint8_t> &buf)
	{
		switch (cmd_id)
		{
		case Bind://�յ�Bind����
			cb->recvBind(buf);
			username = cb->getName();
			password = cb->getPass();
			time_stamp = cb->getTimestamp();
			break;
		case BindResp://�յ�Bind��Ӧ
		{
			resp_ = cr->recvResp(buf);
			authenticator_source_ = cr->getRespInstance()->getAuthenticatorSource();
			shared_secret_ = cr->getRespInstance()->getSharedSecret();
		}
		break;
		case UnBind:
			break;
		case UnBindResp:
			resp_ = cr->recvResp(buf);
			break;
		case SubmitResp://�յ�submit��Ӧ
			resp_ = cr->recvResp(buf);
			ISMG_id_ = cr->getRespInstance()->getISMG_id();
			break;
		case Submit://�յ�submit����
			cs->recvSubmit(buf);
			msg_id_this_ = cs->getMsgId();
			msg_id = cs->getMsgId();
			ph_nums = *(cs->getPhNums());
			msg_text = *(cs->getText());
			pk_total = cs->getSame_msg_total();
			pk_number = cs->getSame_msg_num();

			TP_udhi = cs->getTP_udhi();
			same_msg_total = cs->getSame_msg_total();
			same_msg_num = cs->getSame_msg_num();
			long_msg_id = cs->getLong_msg_id();
			extnum_ = cs->getExtnum();
			break;
		case Deliver://�յ����ж���
			break;
		case DeliverResp://�յ����ж��� ��Ӧ
			dlv->recvDeliverResp(buf);
			msg_id_this_ = dlv->getMsgId();
			result = dlv->getResp_result();
			break;
		case Report://�յ����ű���
			break;
		case ReportResp://�յ����ű��� ��Ӧ

			rp->recvReportResp(buf);
			msg_id_this_ = rp->getMsgId();
			result = rp->getResp_result();
			break;
		case ActiveTest://�յ��������
			break;
		case ActiveTestResp://�յ�������� ��Ӧ
			break;
		default:
			smproxy::exception k("����ָ��ID");
			k.add("[�����ַ���]");
			throw k;
		}
	}
	//ָ��� 1������ 2���Ͽ� 3���ύ��Ϣ 4:���ж��� 5:���ű��� 8:�������
	Ecommand getCmdId() {
		return cmd_id;
	};
	//�û���
	std::string& getUsr() { return username; };
	//����
	std::string& getPas() {
		return password;
	};
	//��ȡʱ���
	unsigned int& getTS() {
		return time_stamp;
	};
	unsigned int& getSN1() {
		return SN1;
	};
	unsigned int& getSN2() {
		return SN2;
	};
	unsigned int& getSN3() {
		return SN3;
	};

	std::string& getSerial_numb() {
		return serial_numb;
	};
	//�������绰����
	std::vector<std::string>& getPhNums() { return ph_nums; };
	//����Ϣ����
	std::string& getMsgText() {
		return msg_text;
	};
	//�Ƿ�Ϊ������ 0���� 1 ��
	int getTP_udhi() {
		return TP_udhi;
	};
	//һ��msg_id��Ӧ����submit
	int getPk_total() {
		return pk_total;
	};
	//һ��msg_id�ĵڼ���submit
	int getPk_number() {
		return pk_number;
	};
	//������������
	int getSame_msg_total() { return same_msg_total; };
	//�����ŵڼ���
	int getSame_msg_num() { return same_msg_num; };
	//������������ű�ʶ
	int getLong_msg_id() {
		return long_msg_id;
	};
	uint32_t getResp() {
		return resp_;
	};
	std::string getMsg_id() {
		return msg_id;
	}
	std::string getMsg_id_this() {
		return msg_id_this_;
	};
	void setMsg_id(std::string msg_id) {
		this->msg_id = msg_id;
	}
	void setResult(uint8_t result) {
		this->result = result;
	}
	uint8_t getResult() {
		return result;
	};

	std::string getExtnum() {
		return extnum_;
	};
private:
	std::string msg_id;
	std::string msg_id_this_;
	uint8_t result;
	//ָ��� 1������ 2���Ͽ� 3���ύ��Ϣ 4:���ж��� 5:���ű��� 8:�������
	Ecommand cmd_id;
	std::string username;
	std::string password;
	unsigned int time_stamp;
	unsigned int SN1;
	unsigned int SN2;
	unsigned int SN3;
	std::vector<std::string> ph_nums;
	std::string msg_text;
	unsigned int resp_;
	size_t total_len;//��Ϣ�ܳ���
	std::string serial_numb; //��Ϣ��ˮ��, ˳���ۼ�, ����Ϊ1, ѭ��ʹ�ã�һ�������Ӧ����Ϣ����ˮ�ű�����ͬ��
							 //std::string source_addr;//Դ��ַ��ΪSP_Id����SP����ҵ���롣cmpp 6�ֽ�

	int pk_total;
	int pk_number;
	int TP_udhi;

	int same_msg_total;
	int same_msg_num;
	int long_msg_id; //������������ű�ʶ

	//CMPP2.0 connect ������������

	std::string authenticator_source_;
	std::string shared_secret_;
	
	std::string ISMG_id_;//����id

	std::string extnum_;//��չ��

						//�ӿ�
	IHead *ch = nullptr;
	IBind *cb = nullptr;
	ISubmit *cs = nullptr;
	IRespAll *cr = nullptr;
	IDeliver *dlv = nullptr;
	IReport *rp = nullptr;

	SGIP12Head sgip12_hd;
	SGIP12Bind sgip12_cb;
	SGIP12Submit sgip12_cs;
	SGIP12Resp sgip12_cr;
	SGIP12Deliver sgip12_dlv;
	SGIP12Report sgip12_rp;
	CMPP20Deliver cmpp20_dlv;
	CMPP20Connect cmpp20_cb;
	CMPP20Head cmpp20_ch;
	CMPP20Submit cmpp20_cs;
	CMPP20Resp cmpp20_cr;

	Eprotocol protocol_;
};