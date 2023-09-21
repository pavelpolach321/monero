// Copyright (c) 2017-2022, The Monero Project
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
//

//#if defined(HAVE_HIDAPI) 
#include <iostream>
#include <vector>
#include <boost/scope_exit.hpp>
#include "log.hpp"
#include "simpleble/SimpleBLE.h"

#include "device_io_ble.hpp"


int test_ble() {
    std::cout << "Using SimpleBLE version: " << SimpleBLE::get_simpleble_version() << std::endl;
    std::cout << "Bluetooth enabled: " << SimpleBLE::Adapter::bluetooth_enabled() << std::endl;

    auto adapter_list = SimpleBLE::Adapter::get_adapters();

    if (adapter_list.empty()) {
        std::cout << "No adapter found" << std::endl;
        return EXIT_FAILURE;
    }

    for (auto& adapter : adapter_list) {
        std::cout << "Adapter: " << adapter.identifier() << " [" << adapter.address() << "]" << std::endl;
    }
    return EXIT_SUCCESS;
}

namespace hw {
  namespace io {
 
    #undef MONERO_DEFAULT_LOG_CATEGORY
    #define MONERO_DEFAULT_LOG_CATEGORY "device.io"
 
    #define ASSERT_X(exp,msg)    CHECK_AND_ASSERT_THROW_MES(exp, msg); 

    device_io_ble::device_io_ble() {
        test_ble();
    }

    void device_io_ble::init(){
      std::cout << " DBG: device_io_ble::init() " << std::endl;
    };

    void device_io_ble::release(){
      std::cout << "\t DBG: device_io_ble::release() " << std::endl;;
    };

    void device_io_ble::connect(void *parms){
      std::cout << "\t DBG: device_io_ble::connect(void *parms) NO ACTION" << std::endl;
    };

    void device_io_ble::disconnect(){
      std::cout << "\t DBG: device_io_ble::disconnect() NO ACTION" << std::endl;
    };

    bool device_io_ble::connected() const {
      std::cout << "\t DBG: device_io_ble::connected() " << std::endl;
      return this->opened;
    };

    int  device_io_ble::exchange(unsigned char *command, unsigned int cmd_len, unsigned char *response_buf, unsigned int max_resp_len, bool user_input){
      std::cout << "\t DBG: device_io_ble::exchange(unsigned char *command, unsigned int cmd_len, unsigned char *response, unsigned int max_resp_len, bool user_input) " << std::endl;
      return 0;
    };



  }
}
 
//#endif //#if defined(HAVE_HIDAPI) 
