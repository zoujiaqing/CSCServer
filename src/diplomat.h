/* 
 * filename : diplomat.h
 * author   : sundayman
 * date     : Jun 13, 2012
 * brief    : 
 */

#ifndef DIPLOMAT_H_
#define DIPLOMAT_H_

#include <string>
#ifndef WIN32
#include <netinet/in.h>
#else
#include <WinSock2.h>
#include <ws2ipdef.h>
#endif

#include "util/ref_counter.h"

#include "def.h"

class Memo : public Utility::RefCounter{

};
typedef Utility::SmartPtr<Memo> MemoPtr;

class Diplomat : public Utility::RefCounter {

public:
	/**
	 * \brief write data to socket.
	 * \return kOK when success, otherwise fail
	 */
	int Speek(const BYTE *content, int len);
	/**
	 * \brief get read data length that no Read by you
	 *
	 */
	size_t GetMassageLength();

	/**
	 * \brief "peekAble" means you can peek data without copy; it can return max length you could peek right now
	 * 			if length is longer than you header of protocol , you can call Peek for efficiency goal;
	 * \return returns max length can return from function Peek.
	 * \remark do not call this function out of the event callback(Embassy::RecvSomething),it is not thread safe
	 */
	size_t GetPeekAbleLength();

	/**
	 * \brief peek data without copy, modify the buffer which is returned is forbidden
	 * \return if success buffer length you have got , otherwise 0
	 */
	size_t Peek(void **buffer);

	/**
	 * \brief read massage from socket.
	 * \param content  massage copy to
	 * \param buffer_length  space of  the content
	 * \param length_expect how much massage expect to read, 0 means read all
	 * \return returns the number of bytes actually read
	 * \remark do not call this function out of the event callback(Embassy::RecvSomething),it is not thread safe
	 */
	size_t ReadMassage(BYTE *content, size_t buffer_length, size_t length_expect);

	//todo: if expect effective , permit to use evbuffer directly

	/**
	 * \brief get socket ip
	 */
	const char* IP() {
		return _ip;
	}

	/**
	 * \brief return socket port
	 */
	unsigned int Port() {
		return _port;
	}

	inline const std::string &Id() {
		return _id;
	}

	void SetMemo(const MemoPtr &memo) {_memo = memo;}
	MemoPtr &GetMemo() {return _memo;}

	void GetMethod(int num) {
		_method_num += num;
	}

	void Dump();
private:
	friend class DiplomatMaster;
	/**
	 * brief forbidden new object except DiplomatMaster
	 */
	Diplomat(struct bufferevent *bev, const std::string &id,
			struct sockaddr *addr, int len);
	/**
	 * brief forbidden delete object except DiplomatMaster
	 */
	~Diplomat();
	/**
	 * brief forbidden copy object
	 */
	Diplomat& operator = (const Diplomat &);
	/**
	 * brief forbidden copy object
	 */
	Diplomat(const Diplomat&);

private:
	struct bufferevent *_bev;
	std::string _id;
	Mutex _mutex_speek;
	char _ip[INET_ADDRSTRLEN + 1];
	unsigned int _port;
	MemoPtr _memo;

	int _method_num;//收到的包个数。
	size_t _recv_cnt;//收到的数据量
	size_t _send_cnt;//发送的数据量
};

typedef Utility::SmartPtr<Diplomat> DiplomatPtr;


#endif /* DIPLOMAT_H_ */
