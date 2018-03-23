#pragma once
/*************************************************
Author:zr
Date:2018-03-5
Description:�쳣�� �������� ��������Ϣ����
**************************************************/

#include <vector>
#include <exception>
#include "boost/exception/all.hpp"


namespace smproxy
{
	//�쳣�� �������� ��������Ϣ����
	class exception :
		virtual public std::exception,
		virtual public boost::exception
	{
	public:
		//@error_str ��������
		exception(std::string error_str) {
			typedef boost::error_info<struct tag_erro_no, int> err_no;
			typedef boost::error_info<struct tag_erro_str, std::string> err_str;
			(*this) << err_str(error_str);
			v_.push_back(error_str);
		};
		//@error_str ��������
		//@error_code �����
		exception(std::string error_str, int error_code) {
			typedef boost::error_info<struct tag_erro_no, int> err_no;
			typedef boost::error_info<struct tag_erro_str, std::string> err_str;
			v_.push_back(error_str);
			this->error_code_ = error_code;
			(*this) << err_str(error_str);
			(*this) << err_no(error_code);
		};
		virtual ~exception() {};
		//��ǰ�˼�����Ϣ
		virtual void add(std::string a) {
			v_.insert(v_.begin(), a);
			std::string str;
			if (!v_.empty())
			{
				for (const auto& i : v_)
				{
					str += i;
				}
			}
			(*this) << boost::error_info<struct tag_erro_str, std::string>(str);
		};
		//��ȡ������Ϣvector
		virtual std::vector<std::string>& get() {
			return v_;
		};
		//��ȡȫ��������Ϣ
		virtual std::string what() {
			std::string str;
			if (!v_.empty())
			{
				for (const auto& i : v_)
				{
					str += i;
				}
			}
			return str;
		};
		virtual void setError_code(int error_code) {
			this->error_code_ = error_code;
		}
		virtual int getError_code() {
			return error_code_;
		}
	protected:
		std::vector<std::string> v_;
		int error_code_ = 9;
	};
}
