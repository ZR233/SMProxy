#pragma once
#include <vector>
#include "../MsgSystem/Cexception.h"
#include <boost/asio.hpp>
#include "Utils.hpp"

/*************************************************
Author:zr
Date:2018-03-15
Description:field element
**************************************************/


namespace smproxy
{
	typedef std::vector<uint8_t> bytes;
	class Field
	{
	public:
		Field(int size) :
			size(size)
		{
		};
		virtual ~Field() {};
		virtual void read(bytes::iterator &iter) = 0;
		virtual bytes toBytes() = 0;
		int size;
	};
	//octet string
	class FieldStr :
		public Field
	{
	public:
		FieldStr(int &size) :
			Field(size)
		{
		}
		~FieldStr() {};
		void set(std::string str)
		{
			this->str_ = str;
			this->str_.resize(size);
		}
		std::string& get()
		{
			return str_;
		}
		void read(bytes::iterator &iter) override
		{
			str_.assign(iter, iter + size);
			str_ = str_.c_str();
			iter += size;
		}
		bytes toBytes() override
		{
			bytes temp;
			temp.assign(str_.begin(), str_.end());
			temp.resize(size);
			return temp;
		}
	protected:
		std::string str_;
	};
	//octet string
	class FieldInt :
		public Field
	{
	public:
		FieldInt(int &size) :
			Field(size)
		{
		}
		~FieldInt() {};

		void set(size_t integer)
		{
			this->integer_ = integer;
		}
		size_t get()
		{
			return integer_;
		}
		void read(bytes::iterator &iter) override
		{
			bytes temp;
			temp.assign(iter, iter + size);
			switch (size)
			{
			case 2:
				{
					uint16_t temp_int;
					memcpy_s(&temp_int, 2, &(temp[0]), 2);
					integer_ = size_t(ntohs(temp_int));
				}
				break;
			case 4:
			{
				uint32_t temp_int;
				memcpy_s(&temp_int, 4, &(temp[0]), 4);
				integer_ = size_t(ntohl(temp_int));
			}
				break;
			case 8:
			{
				memcpy_s(&integer_, 8, &(temp[0]), 8);
				integer_ = Utils::ntohll(integer_);
			}
				break;
			default:
				throw smproxy::exception("错误的数值长度", 99);
			}
			iter += size;
		}
		bytes toBytes() override
		{
			bytes temp(size,0);
			switch (size)
			{
			case 2:
			{
				auto temp_int = htons(uint16_t(integer_));
				memcpy_s(&(temp[0]), 2, &temp_int, 2);
			}
			break;
			case 4:
			{
				auto temp_int = htonl(uint32_t(integer_));
				memcpy_s(&(temp[0]), 4, &temp_int, 4);
			}
			break;
			case 8:
			{
				auto temp_int = Utils::htonll(integer_);
				memcpy_s(&(temp[0]), 8, &temp_int, 8);
			}
			break;
			default:
				throw smproxy::exception("错误的数值长度", 99);
			}
			return temp;
		}
	protected:
		size_t integer_;
	};
	class buffer
	{
	public:
		buffer() 
		{
		};
		~buffer() {};
		void add(smproxy::Field &field)
		{
			auto temp = field.toBytes();
			bytes_.insert(bytes_.end(), temp.begin(), temp.end());
		}
		void reslov(bytes& buf, std::vector<Field*> &section)
		{
			auto iter = buf.begin();
			for (auto field_ptr : section)
			{
				if (iter + field_ptr->size != buf.end())
				{
					field_ptr->read(iter);
				}
				else
				{
					throw smproxy::exception("数据串长度不足", 99);
				}
			}
		}
	private:
		std::vector<uint8_t> bytes_;
	};
}

