// Copyright (c) 2018, The Graft Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include <misc_log_ex.h>
#include <gtest/gtest.h>
#include <inout.h>
#include <jsonrpc.h>
#include <graft_manager.h>
#include <router.h>
#include <requests/getinforequest.h>
#include <requests.h>


#include <string>
#include <thread>
#include <chrono>



using namespace graft;
using namespace std::chrono_literals;

struct CryptonodeHandlersTest : public ::testing::Test
{


    void startServer()
    {
        ServerOpts sopts {"localhost:8855", "localhost:8856", 5.0, 4, 4, "localhost:28881/json_rpc"};
        Router router;
        graft::registerGetInfoRequest(router);
        Manager manager(sopts);
        manager.addRouter(router);
        manager.enableRouting();
        this->manager = &manager;
        server.serve(manager.get_mg_mgr());
    }

    void stopServer()
    {

    }

    CryptonodeHandlersTest()
    {
        mlog_configure("", true);
        mlog_set_log_level(2);
        LOG_PRINT_L0("L0");
        LOG_PRINT_L1("L1");
        LOG_PRINT_L2("L2");

        server_thread = std::thread([this]() {
            this->startServer();
        });

        LOG_PRINT_L0("Server thread started..");

        while (!server.ready) {
            LOG_PRINT_L0("waiting for server");
            std::this_thread::sleep_for(1s);
        }

        LOG_PRINT_L0("Server ready");
    }

    ~CryptonodeHandlersTest()
    {
    }


    static std::string run_cmdline_read(const std::string& cmdl)
    {
        FILE* fp = popen(cmdl.c_str(), "r");
        assert(fp);
        std::string res;
        char path[1035];
        while(fgets(path, sizeof(path)-1, fp))
        {
            res += path;
        }
        pclose(fp);
        return res;
    }
    static std::string escape_string_curl(const std::string & in)
    {
        std::string result;
        for (char c : in) {
            if (c == '"')
                result.push_back('\\');
            result.push_back(c);
        }
        return result;
    }

    static std::string send_request(const std::string &url, const std::string &json_data)
    {
        std::ostringstream s;
        s << "curl -s -H \"Content-type: application/json\" --data \"" << json_data << "\" " << url;
        std::string ss = s.str();
        return run_cmdline_read(ss.c_str());
    }

    static std::string send_request(const std::string &url)
    {
        std::ostringstream s;
        s << "curl -s " << url;
        std::string ss = s.str();
        return run_cmdline_read(ss.c_str());
    }


    static void SetUpTestCase()
    {
    }

    static void TearDownTestCase()
    {
    }

    virtual void SetUp() override
    { }
    virtual void TearDown() override
    { }

    GraftServer server;
    Manager   * manager;
    std::thread server_thread;
};


TEST_F(CryptonodeHandlersTest, getinfo)
{
    MGINFO_YELLOW("*** This test requires running cryptonode RPC on localhost:28881. If not running, test will fail ***");
    std::string response_s = send_request("http://localhost:8855/cryptonode/getinfo");
    EXPECT_FALSE(response_s.empty());

    LOG_PRINT_L2("response: " << response_s);
    Input in; in.load(response_s);
    GetInfoResponse resp = in.get<GetInfoResponse>();
    EXPECT_TRUE(resp.height > 0);
    EXPECT_TRUE(resp.status == "OK");
    EXPECT_TRUE(resp.difficulty > 0);
    EXPECT_TRUE(resp.tx_count > 0);
    LOG_PRINT_L2("Stopping server...");
    manager->stop();
    server_thread.join();
    LOG_PRINT_L2("Server stopped, Server thread done...");
}


