#pragma once
#include "common.h"
#include "buffer.h"
#include "ip_proxy.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <queue>
#include <list>

namespace proxy_ns
{

class TcpProxy;

class TcpConnection
	: private boost::asio::noncopyable
{
public:
	TcpConnection(TcpProxy& proxy, TcpSocket socket);

	~TcpConnection();

	bool start();

	void stop();

	bool isClosed();

	bool flushMsgs(int flush_bytes, global::sec_t expire_time);

private:

	void asyncRead(int pair_id);

	void handleConnect(const boost::system::error_code& ec);

	void handleRead(int pair_id, const boost::system::error_code& ec, std::size_t bytes_transferred);

	void handleWait(const boost::system::error_code& error);

	void handleWrite(int pair_id, const boost::system::error_code& ec);

private:

	bool close_flag;

	TcpProxy& proxy;

	TcpSocket socket_pair[2];

	boost::shared_array<BYTE> recv_buf_pair[2];

	typedef std::pair<global::msec_t, global::Buffer> Msg;
	std::queue<Msg> msg_queue_pair[2];

	int available_bytes_pair[2];

	bool writing_flag_pair[2];

	bool reading_flag_pair[2];

	global::sec_t active_time;

	bool full_open;

	IPProxy *client_ip;
};

typedef boost::shared_ptr<TcpConnection> TcpConn;

//TcpProxy������Ϊ���ɵ���io_service��������˲�����
class TcpProxy
	: private boost::asio::noncopyable
{
public:
	TcpProxy(boost::asio::io_service& io_service, TcpProxyRule proxy_rule);

	virtual ~TcpProxy();

	virtual bool start();

	virtual void stop();

	void getInfo(std::ostream &os);

private:

	void close();

	void startAccept();

	void handleAccept(const boost::system::error_code& ec);

	void asyncWait();

	void handleWait(const boost::system::error_code& error);

	boost::asio::io_service& getIoService()
	{
		return (boost::asio::io_context&)acceptor.get_executor().context();
	}

	friend class TcpConnection;

	const boost::asio::ip::tcp::endpoint& getPassiveDst() const
	{
		return proxy_rule.dst_endpoint;
	}

	int getDelayMs() const
	{
		return proxy_rule.delay_ms;
	}

	int getBandWidth() const
	{
		return proxy_rule.max_bandwidth;
	}

	global::msec_t getNow() const
	{
		return now;
	}

private:

	boost::asio::ip::tcp::acceptor acceptor;

	TcpSocket accept_socket;

	const TcpProxyRule proxy_rule;

	bool close_flag;

	boost::asio::deadline_timer routine_timer;

	global::msec_t now;

	global::msec_t last_flush_tm;

	typedef std::list<TcpConn> TcpConnections;
	TcpConnections tcp_connections;

};

}
