#include <array>
#include <functional>
#include <iostream>
#include <thread>
#define BOOST_ASIO_DISABLE_STD_CHRONO
#include <boost/asio.hpp>
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost;

using namespace std;

#include "Message.hpp"

const int MAX_IP_PACK_SIZE = 65536;// 定义最大IP包大小
const int HEAD_LEN = 4;// 定义头部长度

class RWHandler:public std::enable_shared_from_this<RWHandler>
{
public:
// 构造函数，初始化套接字
    RWHandler(io_service& ios) : m_sock(ios)
    {

    }
    ~RWHandler()
    {

    }
    // 处理读取操作
    void HandleRead()
    { 
        auto self = shared_from_this();
        async_read(m_sock,buffer(m_readMsg.data(),Message::header_length),[this,self](const boost::system::error_code& ec,size_t size){
                            if(ec || !m_readMsg.decode_header())
                            {
                                HandleError(ec);
                                return;
                            }
                            ReadBody();
                        }
        );


    }
    void ReadBody()
    {
        auto self = shared_from_this();
        async_read(m_sock,buffer(m_readMsg.body(),m_readMsg.body_length()),[this,self](const boost::system::error_code& ec,size_t size){
                            if(ec )
                            {
                                HandleError(ec);
                                return;
                            }
                            CallBack(m_readMsg.data(),m_readMsg.length());
                            HandleRead();
                        }
        );
    }
    // 处理写入操作
    void HandleWrite(char* data, int len)
    {
        boost::system::error_code ec;
        if(len>Message::max_body_length)
        {
            return;
        }
        char line[Message::max_body_length]="";
        char header[HEAD_LEN] = {};
        int totalLen = len+HEAD_LEN;
        std::sprintf(header,"%d",totalLen);
        memcpy(line,header,HEAD_LEN);
        memcpy(line+HEAD_LEN,data,len);
        write(m_sock,buffer(line,totalLen),ec);
        // 同步写入数据
        // write(m_sock,buffer(data,len),ec);
        if(ec)
        {
            HandleError(ec);
        }
    }
    // 获取套接字
    tcp::socket& GetSocket()
    {
        return m_sock;
    }
    // 关闭套接字
    void CloseSocket()
    {
        boost::system::error_code ec;
        m_sock.shutdown(tcp::socket::shutdown_send,ec);// 关闭发送方向
        m_sock.close(ec); // 关闭套接字

    }
    // 设置连接ID
    void SetConnId(int connId)
    {
        m_connId = connId;
    }
    // 获取连接ID
    int GetConnId() const
    {
        return m_connId;
    }
    // 设置错误回调函数
    template<typename F>
    void SetCallBackError(F f)
    {
        m_callbackError = f;
    } 
    void CallBack(char* pData,int len)
    {
        cout<<pData+ HEAD_LEN<<endl;
        // HandleWrite(pData,len);
    }
private:
// 处理错误
    void HandleError(const boost::system::error_code& ec)
    {
        CloseSocket();
        cout<<ec.message()<<endl;
        if(m_callbackError)
        {
            m_callbackError(m_connId);
        }
    }
private:
    tcp::socket m_sock;// 套接字
    std::array<char,MAX_IP_PACK_SIZE> m_buff;// 缓冲区
    int m_connId;// 连接ID
    std::function<void(int)> m_callbackError;// 错误回调函数
    Message m_readMsg;

};
