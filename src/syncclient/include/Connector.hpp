#include <array>
#include <functional>
#include <iostream>
#include <thread>
#define BOOST_ASIO_DISABLE_STD_CHRONO
#include <boost/asio.hpp>
#include <boost/thread.hpp>
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost;
#include "RWHandler.hpp"
using namespace std;
class Connector
{
public:
    Connector(io_service& ios,const string& strIP,short port):m_ios(ios),m_socket(ios),
                                                              m_serverAddr(tcp::endpoint(address::from_string(strIP),port)),
                                                              m_chkThread(nullptr)
    {
        CreateEventHandler(ios);
    }
    ~Connector()
    {

    }
    bool Start()
    {
        m_eventHandler->GetSocket().async_connect(m_serverAddr,[this](const boost::system::error_code& error){
                if(error)
                {
                    HandleConnectError(error);
                    return;
                }    
                cout<<"connect ok"<<endl;
                m_isConnected = true;
                m_eventHandler->HandleRead();
            });
            boost::this_thread::sleep(boost::posix_time::seconds(1));
            return m_isConnected;
    }
    bool IsConnected() const
    {
        return m_isConnected;
    }
    void Send(char* data,int len)
    {
        if(!m_isConnected)
        {
            return; 
        }
        m_eventHandler->HandleWrite(data,len);
    }
    // void AsyncSend(char* data,int len)
    // {
    //     if(!m_isConnected)
    //     {
    //         return;
    //     }
    //     m_eventHandler->HandleAsyncWrite(data,len);
    // }
private:
    void CreateEventHandler(io_service& ios)
    {
        m_eventHandler = std::make_shared<RWHandler>(ios);
        m_eventHandler->SetCallBackError([this](int connid){HandleRWError(connid);});
    }
    void CheckConnect()
    {
        if(m_chkThread != nullptr)
        {
            return;
        }
        m_chkThread = std::make_shared<std::thread>([this]{
            while(true)
            {
                if(!IsConnected())
                {
                    Start();
                }
                boost::this_thread::sleep(boost::posix_time::seconds(1));
            }
        });
    }
    void HandleConnectError(const boost::system::error_code& error)
    {
        m_isConnected = false;
        cout<<error.message()<<endl;
        m_eventHandler->CloseSocket();
        CheckConnect();
    }
    void HandleRWError(int connid)
    {
        m_isConnected = false;
        CheckConnect();
    }
private:
    io_service& m_ios;
    tcp::socket m_socket;
    tcp::endpoint m_serverAddr;
    std::shared_ptr<RWHandler> m_eventHandler;
    bool m_isConnected;
    std::shared_ptr<std::thread> m_chkThread;

};