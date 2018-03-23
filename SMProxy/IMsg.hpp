#pragma once
/*************************************************
Author:zr
Date:2018-03-5
Description:消息体接口
**************************************************/


#include <string>
#include <vector>
#include "IHead.hpp"

class IMsg
{
public:
	IMsg(): hd_(nullptr), cmdID(0), resp_result_(0)
	{
	};

	/// <summary>
	/// Gets the MSG identifier.
	/// </summary>
	/// <returns>
	/// string
	/// </returns>
	virtual std::string getMsgId() {
		return msg_id;
	}
	/// <summary>
	/// Gets the MSG identifier this.
	/// </summary>
	/// <returns></returns>
	virtual std::string getMsgIdThis() {
		return msg_id_this;
	}
	virtual uint32_t getResp_result() {
		return resp_result_;
	}
	/// <summary>
	/// Sets the MSG identifier.
	/// </summary>
	/// <param name="msg_id">The MSG identifier.</param>
	virtual void setMsgId(std::string  msg_id)
	{
		this->msg_id = msg_id;
	}
	/// <summary>
	/// Sets the MSG identifier this.
	/// </summary>
	/// <param name="msg_id_this">The MSG identifier this.</param>
	virtual void setMsgIdThis(std::string  msg_id_this)
	{
		this->msg_id_this = msg_id_this;
	}

	/// <summary>
	/// Sets the serial numb.
	///	放入指令流水号字符串，原样返回
	/// </summary>
	/// <param name="serial_numb">The serial numb.</param>
	virtual void setSerialNumb(std::string serial_numb)
	{
		this->serial_numb = serial_numb;
	}

	/// <summary>
	/// Sets the serial numb.
	///	放入指令流水号计数，按协议生成下一个流水号
	/// </summary>
	/// <param name="serial_numb">The serial numb.</param>
	virtual void setSerialNumb(uint32_t serial_numb)
	{
		hd_->setSPID(SPID);
		this->serial_numb = hd_->createSerualNum(serial_numb);
	}

	/// <summary>
	/// Gets the buf.
	/// </summary>
	/// <returns></returns>
	virtual std::vector<uint8_t> getBuf() { return buf_; };

	/// <summary>
	/// Sets the spid.
	///	传入发来的SPID 即SN1
	/// </summary>
	/// <param name="SPID">The spid.</param>
	virtual void setSPID(std::string SPID) {
		this->SPID = SPID;
	};
	virtual std::string getExtnum() {
		return extnum_;
	};
	/**
	* \brief 设置扩展码
	* \param extnum
	*/
	virtual void setExtnum(std::string extnum) {
		extnum_ = extnum;
	};
	/// <summary>
	/// Sets the type of the head.
	/// </summary>
	/// <param name="hd">The hd.</param>
	virtual void setHeadType(IHead *hd) { hd_ = hd; };

	/// <summary>
	/// Adds the head.
	/// </summary>
	virtual void addHead() {
		hd_->header(buf_.size(), cmdID, serial_numb);
		size_t head_len = hd_->getBuf().size();
		hd_->header(head_len + buf_.size(), cmdID, serial_numb);
		buf_.insert(buf_.begin(), hd_->getBuf().begin(), hd_->getBuf().end());
	};
	virtual std::string getISMG_id() {
		return ISMG_id_;
	}
	
	virtual void setISMG_id(std::string ISMG_id)
	{
		this->ISMG_id_ = ISMG_id;
	}
	virtual ~IMsg(){};
protected:
	/// <summary>
	/// The buf
	/// </summary>
	std::vector<uint8_t> buf_;
	/// <summary>
	/// The spid
	/// </summary>
	std::string SPID;
	//brief 扩展码
	std::string extnum_;
	/// <summary>
	/// The hd
	/// </summary>
	IHead* hd_;
	/// <summary>
	/// The command identifier
	/// </summary>
	uint32_t cmdID;
	/// <summary>
	/// The MSG identifier
	/// </summary>
	std::string msg_id;
	/// <summary>
	/// The MSG identifier this
	/// </summary>
	std::string msg_id_this;
	/// <summary>
	/// The serial numb
	/// </summary>
	std::string serial_numb;
	uint32_t resp_result_;
	std::string ISMG_id_;
};
