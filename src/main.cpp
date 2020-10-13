#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <thread>
#include <iostream>
#include <vector>
#include <queue>

#define LOWORD(l) ((uint16_t)(l))
#define HIWORD(l) ((uint16_t)(((uint16_t)(l) >> 16) & 0xFFFF))
#define LOBYTE(w) ((uint8_t)(w))
#define HIBYTE(w) ((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))

#define MAKEWORD(a, b) ((uint16_t)(((uint8_t)(a)) | (((uint16_t)((uint8_t)(b))) << 8)))
#define MAKELONG(a, b) ((uint32_t)(((uint16_t)(a)) | ((uint32_t)((uint16_t)(b))) << 16))

std::string randomStrGen(int length)
{
    static std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    std::string result;
    result.resize(length);

    srand(time(NULL));
    for (int i = 0; i < length; i++)
        result[i] = charset[rand() % charset.length()];

    return result;
}

struct BlockData
{
    BlockData()
        : index(0), data("")
    {
    }

    BlockData(uint32_t index_, std::string data_)
        : index(index_), data(data_)
    {
    }

    uint32_t index;
    std::string data;
};

class BlockChainNode
{
public:
    BlockChainNode()
    {
    }

    virtual ~BlockChainNode()
    {
    }

    void start(uint16_t port)
    {
        std::thread s(&BlockChainNode::server, this, port);
        std::thread c(&BlockChainNode::client, this, port);

        s.join();
        c.join();
    }

    void server(uint16_t port)
    {
        namespace boost_ip = boost::asio::ip;
        boost::asio::io_service io_service;

        // Server binds to any address and any port.
        boost_ip::udp::socket socket(io_service, boost_ip::udp::endpoint(boost_ip::udp::v4(), 0));
        socket.set_option(boost::asio::socket_base::broadcast(true));

        // Broadcast will go to port 8888.
        boost_ip::udp::endpoint broadcast_endpoint(boost_ip::address_v4::broadcast(), port);

        // Broadcast data.
        while (1)
        {
            BlockData new_block;
            if (block_chain.size() == 0)
            {
                new_block = BlockData(0, randomStrGen(6));
            }
            else
            {
                new_block = minor_block(block_chain.back());
            }
            block_chain.push_back(new_block);

            std::vector<uint8_t> buffer(10);
            buffer[0] = 'B';
            buffer[1] = 'L';
            buffer[2] = 'K';
            buffer[3] = 'C';
            buffer[4] = 'N';
            buffer[5] = LOWORD(LOBYTE(new_block.index));
            buffer[5] = LOWORD(HIBYTE(new_block.index));
            buffer[7] = HIWORD(LOBYTE(new_block.index));
            buffer[8] = HIWORD(HIBYTE(new_block.index));

            socket.send_to(boost::asio::buffer(buffer), broadcast_endpoint);
        }
    }

    void client(uint16_t port)
    {
        namespace boost_ip = boost::asio::ip;
        boost::asio::io_service io_service;

        // Client binds to any address on port 8888 (the same port on which
        // broadcast data is sent from server).
        boost_ip::udp::socket socket(io_service, boost_ip::udp::endpoint(boost_ip::udp::v4(), port));
        boost_ip::udp::endpoint sender_endpoint;

        while (1)
        {
            // Receive data.
            boost::array<uint8_t, 100> buffer;
            std::size_t bytes_transferred = socket.receive_from(boost::asio::buffer(buffer), sender_endpoint);

            std::cout << "got " << bytes_transferred << " bytes." << std::endl;
            for (size_t i = 0; i < bytes_transferred; i++)
            {
                std::cout << int(buffer[i]) << " ";
            }
        }
    }

    BlockData minor_block(BlockData pre_block)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return BlockData(pre_block.index + 1, randomStrGen(6));
    }

private:
    std::vector<BlockData> block_chain;
};

int main()
{
    BlockChainNode node;
    node.start(8888);
}
