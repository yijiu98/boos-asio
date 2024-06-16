#include "Connector.hpp"
#include <thread>
#include <string>
#include "addressbook.pb.h"

// 序列化为字节数组
std::pair<char*, int> SerializeToByteArray(const tutorial::AddressBook& address_book) {
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
void ParseFromByteArray(tutorial::AddressBook& address_book, const char* data, int len) {
    if (!address_book.ParseFromArray(data, len)) {
        std::cerr << "Failed to parse address book from byte array." << std::endl;
    }
}
int main(int argc, char **argv)
{
    io_service ios;
    boost::asio::io_service::work work(ios);
    boost::thread thd([&ios]{ios.run();});
    Connector conn(ios,"127.0.0.1",9900);
    conn.Start();
    std::string str;
    if(!conn.IsConnected())
    {
        // cin>>str;
        return -1;
    }

    while(1)
    {
        // char arr[]="hello world";
        // conn.Send(arr,sizeof(arr)/sizeof(arr[0]));
        // std::this_thread::sleep_for(std::chrono::seconds(2));

        // char data[]={0x01,0x02,0x03};
        // conn.Send(data,sizeof(data)/sizeof(data[0]));
        // std::this_thread::sleep_for(std::chrono::seconds(2));


        // 创建一个Person对象
        tutorial::Person* person = new tutorial::Person();
        person->set_id(234);
        person->set_name("John Doe");
        person->set_email("johndoe@example.com");

        // 创建一个AddressBook对象
        tutorial::AddressBook address_book;
        address_book.add_people()->CopyFrom(*person);

        // 序列化到字节数组
        auto serialized = SerializeToByteArray(address_book);
        char* serialized_data = serialized.first;
        int serialized_len = serialized.second;
        // 打印序列化数据的大小
        // std::cout << "serialized_data:"<<serialized_data<<","<<"Serialized data size: " << serialized_data.size() << " bytes" << std::endl;
        conn.Send(serialized_data,serialized_len);
        delete person;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    
    exit(1);
}