#include <aquarius.hpp>
#include <iostream>
#include <srvd_client.hpp>
#include "global_config.hpp"

namespace po = boost::program_options;
using namespace aquarius::gateway;

int main(int argc, char* argv[])
{
    po::options_description cmd("gateway");

    std::string name("gateway");
    int32_t pool_size = static_cast<int32_t>(std::thread::hardware_concurrency());

    cmd.add_options()("help", "print help message");
    cmd.add_options()("listen", po::value<uint16_t>(), "listen port");
    cmd.add_options()("pool_size", po::value<int32_t>(&pool_size), "pool thread size");
    cmd.add_options()("name", po::value<std::string>(&name), "server name");
    cmd.add_options()("proto", po::value<std::string>(), "proto that tcp and http1.1");
    cmd.add_options()("srvd_host", po::value<std::string>(), "serviced host");
    cmd.add_options()("srvd_port", po::value<uint16_t>(), "serviced port");

    uint16_t port{};

    po::variables_map vm{};
    po::store(po::parse_command_line(argc, argv, cmd), vm);
    po::notify(vm);

    auto& proto = global_config::get_mutable_instance().proto;


    if (vm.count("help"))
    {
        std::cout << cmd;
        return 0;
    }

    if (vm.count("listen"))
    {
        port = vm["listen"].as<uint16_t>();
    }
    else
    {
        std::cout << "the listen port is invalid\n";
        std::cout << cmd;
        return 0;
    }

    if (vm.count("pool_size"))
    {
        pool_size = vm["pool_size"].as<int32_t>();
    }

    if (vm.count("name"))
    {
        name = vm["name"].as<std::string>();
    }

    if (vm.count("proto"))
    {
        global_config::get_mutable_instance().proto = vm["proto"].as<std::string>();
    }
    else
    {
        std::cout << "the proto is invalid\n";
        std::cout << cmd;
        return 0;
    }

    if (vm.count("srvd_host"))
    {
        srv_config::get_mutable_instance().host = vm["srvd_host"].as<std::string>();
    }
    else
    {
        std::cout << "the srvd_host is invalid\n";
        std::cout << cmd;
        return 0;
    }

    if (vm.count("srvd_port"))
    {
        srv_config::get_mutable_instance().port = vm["srvd_port"].as<uint16_t>();
    }
    else
    {
        std::cout << "the srvd_port is invalid\n";
        std::cout << cmd;
        return 0;
    }

    if (proto == "tcp")
    {
        aquarius::tcp::server server(port, pool_size, name);

        server.run();
    }
    else if (proto == "http1.1")
    {
        aquarius::http::server server(port, pool_size, name);

        server.run();
    }
    else
    {
        std::cout << "the proto [" << proto << "] not support!\n";
    }


    return 0;
}
