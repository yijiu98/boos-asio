// #include <boost/asio/bufffer.hpp>
#include <unordered_map>
#include <numeric>
#include <list>
// #include "Message.hpp"
#include "RWHandler.hpp"
#include "addressbook.pb.h"
const int MaxConnectionNum = 65536;// 定义最大连接数
const int MaxRecvSize = 65536;// 定义最大接收大小
class Server
{
public:
// 构造函数，初始化 io_service 和 acceptor，并填充连接ID池
    Server(io_service& ios,short port):m_ios(ios),m_acceptor(ios,tcp::endpoint(tcp::v4(),port))
    {
        m_connIdPool.resize(MaxConnectionNum);
        std::iota(m_connIdPool.begin(),m_connIdPool.end(),1);//填充列表,从1开始一次递增
    }
    ~Server()
    {}
    // 开始接受连接
    void Accept()
    {
        cout<<"start Listening "<<endl;
        std::shared_ptr<RWHandler> handler = CreateHandler();// 创建一个新的处理器
        // 异步接受连接
        m_acceptor.async_accept(handler->GetSocket(),[this,handler](const boost::system::error_code& error){
                            if(error)
                            {
                                
                                cout<<"async_accept eror"<<error.value()<<" "<<error.message()<<endl;
                                HandleAcpError(handler,error);
                                return;
                            }
                            m_handlers.insert(std::make_pair(handler->GetConnId(),handler));// 插入新的连接处理器
                            cout<<"current connect count:"<<m_handlers.size()<<endl;
                            handler->HandleRead();// 开始读取数据
                            Accept();// 继续接受新的连接
                        });
    }
private:
    // 处理接受错误
    void HandleAcpError(std::shared_ptr<RWHandler> eventHanlder,const boost::system::error_code& error)
    {
        cout<<"Error , error reason:" <<error.value()<<error.message()<<endl;
        eventHanlder->CloseSocket();// 关闭错误的连接
        StopAccept();// 停止接受新的连接
    }
    // 停止接受新的连接
    void StopAccept()
    {
        boost::system::error_code ec;
        m_acceptor.cancel(ec);// 取消所有挂起的异步操作
        m_acceptor.close(ec);// 关闭接受器
        m_ios.stop();// 停止 io_service
    }
    // 创建一个新的处理器
    std::shared_ptr<RWHandler> CreateHandler()
    {
        int connId = m_connIdPool.front();// 从连接ID池中取出一个ID
        m_connIdPool.pop_front();
        std::shared_ptr<RWHandler> handler = std::make_shared<RWHandler> (m_ios);// 创建新的RWHandler
        handler->SetConnId(connId);// 设置连接ID
        handler->SetCallBackError([this](int connId){// 设置错误回调函数
            RecyclConnid(connId);// 回收连接ID
        });
        handler->regsRecvCbk([this](char* data, int len) {
            tutorial::AddressBook new_address_book;
            std::cout << "Received data of length: " << len << std::endl;
            ParseFromByteArray(new_address_book, data, len);

            // 输出反序列化的数据
            if (new_address_book.people_size() > 0) {
                std::cout << "Person ID: " << new_address_book.people(0).id() << std::endl;
                std::cout << "Person Name: " << new_address_book.people(0).name() << std::endl;
                std::cout << "Person Email: " << new_address_book.people(0).email() << std::endl;
            } else {
                std::cout << "No people in address book." << std::endl;
            }
        });
        return handler;
    }
    // 回收连接ID
    void RecyclConnid(int connId)
    {
        auto it = m_handlers.find(connId);// 查找连接ID对应的处理器
        if(it != m_handlers.end())
        {
            m_handlers.erase(it);// 移除处理器
        }
        cout<<"current connect count: "<<m_handlers.size()<<endl;
        m_connIdPool.push_back(connId);// 将连接ID放回连接ID池
    }

    // 序列化为字节数组
    std::pair<char*, int> SerializeToByteArray(const tutorial::AddressBook& address_book) 
    {
        std::string serialized_data;
        if (!address_book.SerializeToString(&serialized_data)) {
            std::cerr << "Failed to serialize address book." << std::endl;
            return {nullptr, 0};
        }

        int len = serialized_data.size();
        char* byte_array = new char[len];
        std::memcpy(byte_array, serialized_data.data(), len);

        return {byte_array, len};
    }

    // 从字节数组反序列化
    void ParseFromByteArray(tutorial::AddressBook& address_book, const char* data, int len) 
    {
        if (!data || len <= 0) {
            std::cerr << "Invalid input data or length." << std::endl;
            return;
        }
        if (!address_book.ParseFromArray(data, len)) {
            std::cerr << "Failed to parse address book from byte array." << std::endl;
        } else {
            std::cout << "Successfully parsed address book from byte array." << std::endl;
        }
    }
private:
    io_service& m_ios;// io_service 对象的引用
    tcp::acceptor m_acceptor;// TCP接受器
    std::unordered_map<int,std::shared_ptr<RWHandler>> m_handlers; // 存储连接处理器的哈希表
    std::list<int> m_connIdPool; // 连接ID池

};