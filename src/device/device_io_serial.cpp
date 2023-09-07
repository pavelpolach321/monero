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

#include <boost/scope_exit.hpp>
#include "log.hpp"
#include "device_io_serial.hpp"
#include "CSerialPort/SerialPortInfo.h"


#include <iostream>

namespace hw {
  namespace io {
 
    #undef MONERO_DEFAULT_LOG_CATEGORY
    #define MONERO_DEFAULT_LOG_CATEGORY "device.io"
 
    #define ASSERT_X(exp,msg)    CHECK_AND_ASSERT_THROW_MES(exp, msg); 

    device_io_serial::device_io_serial() {}

    void device_io_serial::init(){
      std::cout << " DBG: device_io_serial::init() " << std::endl;
      const char portName[] = "/dev/ttyUSB0";
      sp.init(portName,                // windows:COM1 Linux:/dev/ttyS0
              itas109::BaudRate115200, // baudrate
              itas109::ParityNone,     // parity
              itas109::DataBits8,      // data bit
              itas109::StopOne,        // stop bit
              itas109::FlowNone,       // flow
              4096                     // read buffer size
      );
      sp.setReadIntervalTimeout(0); // read interval timeout 0ms

      this->opened = sp.open();
    };

    void device_io_serial::release(){
      std::cout << "\t DBG: device_io_serial::release() " << std::endl;
      sp.close();
      this->opened = 0;
    };

    void device_io_serial::connect(void *parms){
      std::cout << "\t DBG: device_io_serial::connect(void *parms) NO ACTION" << std::endl;
    };

    void device_io_serial::disconnect(){
      std::cout << "\t DBG: device_io_serial::disconnect() NO ACTION" << std::endl;
    };

    bool device_io_serial::connected() const {
      std::cout << "\t DBG: device_io_serial::connected() " << std::endl;
      return this->opened;
    };

    int  device_io_serial::exchange(unsigned char *command, unsigned int cmd_len, unsigned char *response_buf, unsigned int max_resp_len, bool user_input){
      std::cout << "\t DBG: device_io_serial::exchange(unsigned char *command, unsigned int cmd_len, unsigned char *response, unsigned int max_resp_len, bool user_input) " << std::endl;

      if(this->opened) {
        sp.writeData(command, cmd_len);
        unsigned int received = 0;
        unsigned int cycles = 0;
        unsigned int max_cycles = 5;
        unsigned char buff;
        while(received < max_resp_len && cycles < max_cycles) {
          if(sp.readData(&buff, 1)) {
            //std::cout << "rcvd char \r\n" << buff << std::endl;
            *(response_buf + received) = buff;
            //response++;
            received++;
          } else {
            //std::cout << "usleep cycle \r\n" << std::endl;
            usleep(1000 * 100);
            cycles++;
          }
          
        }
      }
      MDEBUG(" debug: ");
      std::cout << "sleep stop\r\n" << std::endl;
        
      return 0;
    };



  }
}
