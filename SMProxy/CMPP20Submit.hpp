#pragma once
#include "ISubmit.hpp"
#include "CMPP20Head.hpp"

/*************************************************
Author:zr
Date:2018-01-17
Description:CMPP submit����
**************************************************/



/**
* \brief ���ڵݽ�����
*/
class CMPP20Submit :
	public ISubmit
{
public:
	CMPP20Submit() :ISubmit(4)
	{
		setHeadType(&hd_);
	}
	virtual ~CMPP20Submit(){};
	// ����submit buf
	bytes submit(std::vector<std::string> &user_num, std::string &msg) override
	{
		if (user_num.size() > 100 || user_num.size() < 1)
			throw smproxy::exception("�ֻ�����������");
		std::string msg_str = msg.c_str();
		if (msg_str.size() > 140)
			throw smproxy::exception("�������ݹ���");

		buf_.assign(8, 0);
		buf_.push_back(pk_total_);
		buf_.push_back(pk_number_);
		buf_.push_back(report_flag_);
		buf_.push_back(msg_level_);
		service_id_.resize(10);
		buf_.insert(buf_.end(), service_id_.begin(), service_id_.end());
		buf_.push_back(fee_usertype_);
		fee_number_.resize(21);
		buf_.insert(buf_.end(), fee_number_.begin(), fee_number_.end());
		buf_.push_back(tp_pid_);
		buf_.push_back(tp_udhi_);
		buf_.push_back(msg_fmt_);
		extnum_.resize(6);
		buf_.insert(buf_.end(), extnum_.begin(), extnum_.end());
		fee_type_.resize(2);
		buf_.insert(buf_.end(), fee_type_.begin(), fee_type_.end());
		fee_code_.resize(6);
		buf_.insert(buf_.end(), fee_code_.begin(), fee_code_.end());
		valid_time_.resize(17);
		buf_.insert(buf_.end(), valid_time_.begin(), valid_time_.end());
		at_time_.resize(17);
		buf_.insert(buf_.end(), at_time_.begin(), at_time_.end());
		sp_number_.resize(21);
		buf_.insert(buf_.end(), sp_number_.begin(), sp_number_.end());
		dest_total_ = user_num.size();
		buf_.push_back(dest_total_);

		for (auto i = 0; i < user_num.size(); i++)
		{
			bytes temp_ph;
			temp_ph.assign(user_num[i].begin(), user_num[i].end());
			temp_ph.resize(21);
			buf_.insert(buf_.end(), temp_ph.begin(), temp_ph.end());
		}

		message_length_ = msg_str.size();
		buf_.push_back(uint8_t(message_length_));
		buf_.insert(buf_.end(), msg_str.begin(), msg_str.end());
		for (size_t i = 0; i < 8; i++)
		{
			buf_.push_back('\0');
		}
	}
	// ����submit
	void recvSubmit(bytes &buf) override {
		auto iter = buf.begin();
		iter += 12;
		iter += 8;
		pk_total_ = *iter;
		++iter;
		pk_number_ = *iter;
		++iter;
		report_flag_ = *iter;
		++iter;
		msg_level_ = *iter;
		++iter;

		service_id_ = service_id_.assign(iter, iter + 10).c_str();
		iter += 10;
		fee_usertype_ = *iter;
		++iter;

		fee_number_ = fee_number_.assign(iter, iter + 21).c_str();
		iter += 21;

		tp_pid_ = *iter;
		++iter;

		tp_udhi_ = *iter;
		++iter;
		msg_fmt_ = *iter;
		++iter;

		extnum_ = extnum_.assign(iter, iter + 6).c_str();
		iter += 6;

		fee_type_ = fee_type_.assign(iter, iter + 2).c_str();
		iter += 2;

		fee_code_ = fee_code_.assign(iter, iter + 6).c_str();
		iter += 6;

		valid_time_ = valid_time_.assign(iter, iter + 17).c_str();
		iter += 17;

		at_time_ = at_time_.assign(iter, iter + 17).c_str();
		iter += 17;

		extnum_ = extnum_.assign(iter, iter + 21).c_str();
		iter += 21;


		dest_total_ = *iter;
		++iter;



		if (buf.size() < (116 + 21 * dest_total_ + 1))
		{
			throw smproxy::exception("submit��Ϣ���ȴ���", 1);
		}
		//�����ֻ���
		if (dest_total_ >100)
		{
			throw smproxy::exception("�ֻ��Ź���", 9);
		}
		num_.clear();
		for (auto i = 0; i <dest_total_; i++)
		{
			std::string tempstr;
			tempstr.assign(iter, iter + 21);
			tempstr = tempstr.c_str();
			if (tempstr.size()>11)
			{
				throw smproxy::exception("�ֻ���ʽ��", 9);
			}
			num_.push_back(tempstr);
			iter += 21;
		}

		message_length_ = uint32_t(*iter);
		if (int(message_length_)>140)
		{
			throw smproxy::exception("�������ݹ���", 6);
		}
		++iter;
		if (buf.size() < (116 + 21 * dest_total_ + 1 + int(message_length_)))
		{
			throw smproxy::exception("submit��Ϣ���ȴ���", 1);
		}

		//�����Ŵ���-------------------------------------------------

		int min = 0;
		if (tp_udhi_ == 1)
		{
			//byte 1 : 05, ��ʾʣ��Э��ͷ�ĳ���
			min = (uint32_t)(uint8_t)(*iter) + 1;
			++iter;
			//byte 2 : 00, ���ֵ��GSM 03.40�淶9.2.3.24.1�й涨����ʾ���������������ŵı�ʶλ����Ϊ1����ʽ�е�XXֵ����
			//byte 2 : 08, ���ֵ��GSM 03.40�淶9.2.3.24.1�й涨����ʾ���������������ŵı�ʶλ����Ϊ2����ʽ�е�XXֵ����
			uint8_t c = *iter;
			++iter;
			++iter;
			//byte 2 : 00, ���ֵ��GSM 03.40�淶9.2.3.24.1�й涨����ʾ���������������ŵı�ʶλ����Ϊ1����ʽ�е�XXֵ����
			if (c == 0)
			{
				long_msg_id_ = (uint32_t)(uint8_t)(*iter);
				++iter;
			}
			//byte 2 : 08, ���ֵ��GSM 03.40�淶9.2.3.24.1�й涨����ʾ���������������ŵı�ʶλ����Ϊ2����ʽ�е�XXֵ����
			else if (c == 8)
			{
				uint16_t d = 0;
				memcpy_s(&d, 2, &(*iter), 2);
				iter += 2;
				long_msg_id_ = (uint32_t)d;
			}
			else
			{
				throw smproxy::exception("����������ͷ��ʽ����", 1);
			}
			same_msg_total_ = (uint8_t)*iter;
			++iter;
			same_msg_num_ = (uint8_t)*iter;
			++iter;
		}
		//������Ϣ����-----------------------------------
		switch (msg_fmt_)
		{
		case 8:
		case 9:
		{

			int nDstLength;        // UNICODE���ַ���Ŀ
			WCHAR wchar[200] = { 0 };  // UNICODE��������
			std::wstring wstr(400, L'\0');
			// �ߵ��ֽڶԵ���ƴ��UNICODE
			for (int i = 0; i < (message_length_ - min) / 2; i++)
			{
				wstr[i] = uint16_t(*iter++) << 8;    // �ȸ�λ�ֽ�
				wstr[i] |= *iter++;        // ���λ�ֽ�
			}
			wstr = wstr.c_str();
			// UNICODE��-->�ַ���
			message_content_ = boost::locale::conv::from_utf<wchar_t>(wstr, "GBK");
		}
		break;
		case 15:
			message_content_.assign(iter, iter + message_length_ - min);
			break;
		default:
			throw smproxy::exception("δʶ������", 9);
		}
	}


	// ����submit buf 
	bytes submit(std::vector<std::string> &user_num, uint8_t msg_fmt, std::vector<uint8_t> &msg) override
	{
		buf_.assign(8, 0);
		buf_.push_back(pk_total_);
		buf_.push_back(pk_number_);
		buf_.push_back(report_flag_);
		buf_.push_back(msg_level_);
		service_id_.resize(10);
		buf_.insert(buf_.end(), service_id_.begin(), service_id_.end());
		buf_.push_back(fee_usertype_);
		fee_number_.resize(21);
		buf_.insert(buf_.end(), fee_number_.begin(), fee_number_.end());
		buf_.push_back(tp_pid_);
		buf_.push_back(tp_udhi_);
		buf_.push_back(msg_fmt_);
		extnum_.resize(6);
		buf_.insert(buf_.end(), extnum_.begin(), extnum_.end());
		fee_type_.resize(2);
		buf_.insert(buf_.end(), fee_type_.begin(), fee_type_.end());
		fee_code_.resize(6);
		buf_.insert(buf_.end(), fee_code_.begin(), fee_code_.end());
		valid_time_.resize(17);
		buf_.insert(buf_.end(), valid_time_.begin(), valid_time_.end());
		at_time_.resize(17);
		buf_.insert(buf_.end(), at_time_.begin(), at_time_.end());
		sp_number_.resize(21);
		buf_.insert(buf_.end(), sp_number_.begin(), sp_number_.end());
		dest_total_ = user_num.size();
		buf_.push_back(dest_total_);

		//�����ֻ���----------
		if (user_num.size() > 100)
		{
			throw std::runtime_error("submit �ֻ��ų���100��");
		}
		for (int i = 0; i < user_num.size(); i++)
		{
			bytes temp_ph;
			temp_ph.assign(user_num[i].begin(), user_num[i].end());
			temp_ph.resize(21);
			buf_.insert(buf_.end(), temp_ph.begin(), temp_ph.end());
		}
		//������Ϣ
		if (msg.size() > 140)
		{
			throw std::runtime_error("submit �������ݳ���140");
		}
		message_length_ = (msg.size());
		buf_.push_back(uint8_t(message_length_));
		buf_.insert(buf_.end(), msg.begin(), msg.end());
		for (size_t i = 0; i < 8; i++)
		{
			buf_.push_back('\0');
		}
	}

private:
	//SMSPod smsp;
	CMPP20Head hd_;
};

