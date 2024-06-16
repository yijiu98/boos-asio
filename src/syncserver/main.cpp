#include "Server.hpp"
void TestServer()
{
    io_service ios;
    // boost::asio::io_service::work work(ios);
    // std::thread thd([&ios]{ios.run();});
    Server server(ios,9900);
    server.Accept();
    ios.run();
    // thd.join();
}
int main(int argc, char **argv)
{
    
    TestServer();
    while(1)
    {
        cout<<"server running..."<<endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    exit(1);
}