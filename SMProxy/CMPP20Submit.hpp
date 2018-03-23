#pragma once
#include "ISubmit.hpp"
#include "CMPP20Head.hpp"

/*************************************************
Author:zr
Date:2018-01-17
Description:CMPP submit处理
**************************************************/



/**
* \brief 用于递交短信
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
	// 生成submit buf
	bytes submit(std::vector<std::string> &user_num, std::string &msg) override
	{
		if (user_num.size() > 100 || user_num.size() < 1)
			throw smproxy::exception("手机号数量错误");
		std::string msg_str = msg.c_str();
		if (msg_str.size() > 140)
			throw smproxy::exception("短信内容过长");

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
	// 接收submit
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
			throw smproxy::exception("submit消息长度错误", 1);
		}
		//拷贝手机号
		if (dest_total_ >100)
		{
			throw smproxy::exception("手机号过多", 9);
		}
		num_.clear();
		for (auto i = 0; i <dest_total_; i++)
		{
			std::string tempstr;
			tempstr.assign(iter, iter + 21);
			tempstr = tempstr.c_str();
			if (tempstr.size()>11)
			{
				throw smproxy::exception("手机格式错", 9);
			}
			num_.push_back(tempstr);
			iter += 21;
		}

		message_length_ = uint32_t(*iter);
		if (int(message_length_)>140)
		{
			throw smproxy::exception("短信内容过长", 6);
		}
		++iter;
		if (buf.size() < (116 + 21 * dest_total_ + 1 + int(message_length_)))
		{
			throw smproxy::exception("submit消息长度错误", 1);
		}

		//长短信处理-------------------------------------------------

		int min = 0;
		if (tp_udhi_ == 1)
		{
			//byte 1 : 05, 表示剩余协议头的长度
			min = (uint32_t)(uint8_t)(*iter) + 1;
			++iter;
			//byte 2 : 00, 这个值在GSM 03.40规范9.2.3.24.1中规定，表示随后的这批超长短信的标识位长度为1（格式中的XX值）。
			//byte 2 : 08, 这个值在GSM 03.40规范9.2.3.24.1中规定，表示随后的这批超长短信的标识位长度为2（格式中的XX值）。
			uint8_t c = *iter;
			++iter;
			++iter;
			//byte 2 : 00, 这个值在GSM 03.40规范9.2.3.24.1中规定，表示随后的这批超长短信的标识位长度为1（格式中的XX值）。
			if (c == 0)
			{
				long_msg_id_ = (uint32_t)(uint8_t)(*iter);
				++iter;
			}
			//byte 2 : 08, 这个值在GSM 03.40规范9.2.3.24.1中规定，表示随后的这批超长短信的标识位长度为2（格式中的XX值）。
			else if (c == 8)
			{
				uint16_t d = 0;
				memcpy_s(&d, 2, &(*iter), 2);
				iter += 2;
				long_msg_id_ = (uint32_t)d;
			}
			else
			{
				throw smproxy::exception("长短信内容头格式错误", 1);
			}
			same_msg_total_ = (uint8_t)*iter;
			++iter;
			same_msg_num_ = (uint8_t)*iter;
			++iter;
		}
		//拷贝信息内容-----------------------------------
		switch (msg_fmt_)
		{
		case 8:
		case 9:
		{

			int nDstLength;        // UNICODE宽字符数目
			WCHAR wchar[200] = { 0 };  // UNICODE串缓冲区
			std::wstring wstr(400, L'\0');
			// 高低字节对调，拼成UNICODE
			for (int i = 0; i < (message_length_ - min) / 2; i++)
			{
				wstr[i] = uint16_t(*iter++) << 8;    // 先高位字节
				wstr[i] |= *iter++;        // 后低位字节
			}
			wstr = wstr.c_str();
			// UNICODE串-->字符串
			message_content_ = boost::locale::conv::from_utf<wchar_t>(wstr, "GBK");
		}
		break;
		case 15:
			message_content_.assign(iter, iter + message_length_ - min);
			break;
		default:
			throw smproxy::exception("未识别编码号", 9);
		}
	}


	// 生成submit buf 
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

		//拷贝手机号----------
		if (user_num.size() > 100)
		{
			throw std::runtime_error("submit 手机号超过100个");
		}
		for (int i = 0; i < user_num.size(); i++)
		{
			bytes temp_ph;
			temp_ph.assign(user_num[i].begin(), user_num[i].end());
			temp_ph.resize(21);
			buf_.insert(buf_.end(), temp_ph.begin(), temp_ph.end());
		}
		//拷贝信息
		if (msg.size() > 140)
		{
			throw std::runtime_error("submit 短信内容超过140");
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

