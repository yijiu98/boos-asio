#include <iostream>
#include <cstring>  // for std::strncat
#pragma once
class Message
{
public:
    enum{header_length =4};// 定义消息头长度为4字节
    enum{max_body_length = 512};// 定义消息体最大长度为512字节
    Message():body_length_(0)// 构造函数，初始化消息体长度为0
    {

    }
    const char* data() const// 返回包含头部和消息体的整个数据指针（常量版本）
    {
        return data_;
    }
    char* data()// 返回包含头部和消息体的整个数据指针（非常量版本）
    {
        return data_;
    }
    size_t length() const// 返回包含头部和消息体的整个数据长度
    {
        return header_length + body_length_;
    }
    const char* body() const// 返回消息体数据的指针（常量版本）
    {
        return data_ + header_length;
    }
    char* body()// 返回消息体数据的指针（非常量版本）
    {
        return data_ + header_length;
    }
    size_t body_length() const// 返回消息体的长度
    {
        return body_length_;
    }
    void body_length(size_t new_length) // 设置消息体长度，如果新长度大于最大长度，则截断
    {
        body_length_ = new_length;
        if(body_length_ > max_body_length)
        {
            body_length_ = max_body_length;
        }
    }
    bool decode_header()// 解码头部，解析出消息体长度
    {
        char header[header_length + 1] = ""; // 创建一个额外字节用于存储终止符
        std::strncat(header,data_,header_length);// 将头部数据拷贝到header数组中,（data_放在header后面）
        body_length_ = std::atoi(header) - header_length; // 将头部数据转换为整数，并减去头部长度
        if(body_length_ > max_body_length)// 如果消息体长度超过最大长度，重置为0并返回false
        {
            body_length_ = 0;
            return false;
        }
        std::cout << "Decoded body_length: " << body_length_ << std::endl;
        return true;
    }
    // 编码头部，将消息体长度写入头部
    void encode_header()
    {
        char header[header_length+1]=""; // 创建一个额外字节用于存储终止符
        std::sprintf(header,"%4d",body_length_);// 将消息体长度格式化为4位宽度的字符串
        std::memcpy(data_,header,header_length);//将头部数据拷贝到消息数组的前4个字节
    }
private:
    char data_[header_length + max_body_length]; // 存储头部和消息体的数据数组
    std::size_t body_length_; // 消息体长度

};