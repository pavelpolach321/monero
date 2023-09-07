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

#include "version.h"
#include "device_rmx.hpp"
#include "ringct/rctOps.h"
#include "cryptonote_basic/account.h"
#include "cryptonote_basic/subaddress_index.h"
#include "cryptonote_core/cryptonote_tx_utils.h"

#include <boost/thread/locks.hpp> 
#include <boost/thread/lock_guard.hpp>


namespace hw {

  namespace rmx {

    #undef MONERO_DEFAULT_LOG_CATEGORY
    #define MONERO_DEFAULT_LOG_CATEGORY "device.rmx"

    #define HW_RMX_NAME "rmx"
    
    //static device_rmx *rmx_device = nullptr;

//    void register_all(std::map<std::string, std::unique_ptr<device>> &registry) {
//      if (!rmx_device) {
//        rmx_device = new device_rmx();
//        rmx_device->set_name("rmx");
//      }
//      registry.insert(std::make_pair("rmx", std::unique_ptr<device>(rmx_device)));
//    }
//
//    void register_all() {
//      if (!rmx_device) {
//        rmx_device = new device_rmx();
//        rmx_device->set_name("rmx");
//      }
//      //
//
//       get registry here?
//
//      //
//      registry.insert(std::make_pair("rmx", std::unique_ptr<device>(rmx_device)));
//    }
//

    static device_rmx *rmx_device = nullptr;
    static device_rmx *ensure_rmx_device(){
      if (!rmx_device) {
        rmx_device = new device_rmx();
        rmx_device->set_name(HW_RMX_NAME);
      }
      return rmx_device;
    }

    void register_all() {
      hw::register_device(HW_RMX_NAME, ensure_rmx_device());
    }


    device_rmx::device_rmx(): hw_device() {
      //this->id = device_id++;
      //this->reset_buffer();      
      //this->mode = NONE;
      //this->has_view_key = false;
      //this->tx_in_progress = false;
      //MDEBUG( "Device "<<this->id <<" Created");
      std::cout << " DBG: device_rmx::device_rmx()" << std::endl;
    }

    device_rmx::~device_rmx() {
      //this->release();
      //MDEBUG( "Device "<<this->id <<" Destroyed");
      this->release();
      std::cout << "... called Device RMX destructor " << std::endl;
    }

    bool device_rmx::connected() {
      return hw_device.connected();
    }

    unsigned int device_rmx::exchange(unsigned int ok, unsigned int mask) {
      this->length_recv =  hw_device.exchange(this->buffer_send, this->length_send, this->buffer_recv, 262, false);
      this->sw = (this->buffer_recv[length_recv]<<8) | this->buffer_recv[length_recv+1];
      return this->sw;
    }

    bool device_rmx::set_name(const std::string &name){
      this->name = name;
      std::cout << " DBG: device_rmx::set_name(): " << this->name << std::endl;
      return true;
    };

    const std::string device_rmx::get_name() const {
      if (!hw_device.connected()) {
        return std::string("<disconnected:").append(this->name).append(">");
      }
      return this->name;
    };

    bool device_rmx::init(){
      std::cout << "\t DBG: device_rmx::init()" << std::endl;
      hw_device.init();
      return hw_device.connected();
    };

    bool device_rmx::release(){
      std::cout << "\t DBG: device_rmx::release()" << std::endl;
      hw_device.release();
      return true;
    };

    //    IN FACT NO ACTION HERE, PORT IS ONLY INIT OR RELEASED ^^
    bool device_rmx::connect(){
      std::cout << "\t DBG: device_rmx::connect()" << std::endl;
      hw_device.connect(NULL);
      return true;
    };
    //    IN FACT NO ACTION HERE, PORT IS ONLY INIT OR RELEASED ^^
    bool device_rmx::disconnect(){
      std::cout << "\t DBG: device_rmx::disconnect()" << std::endl;
      hw_device.disconnect();
      return true;
    };

    /* ======================================================================= */
    /*  LOCKER copied from ledger                                               */
    /* ======================================================================= */ 
    
    //automatic lock one more level on device ensuring the current thread is allowed to use it
    #define AUTO_LOCK_CMD() \
      /* lock both mutexes without deadlock*/ \
      boost::lock(device_locker, command_locker); \
      /* make sure both already-locked mutexes are unlocked at the end of scope */ \
      boost::lock_guard<boost::recursive_mutex> lock1(device_locker, boost::adopt_lock); \
      boost::lock_guard<boost::mutex> lock2(command_locker, boost::adopt_lock)

    //lock the device for a long sequence
    void device_rmx::lock(void) {
      MDEBUG( "Ask for LOCKING for device "<<this->name << " in thread ");
      device_locker.lock();
      MDEBUG( "Device "<<this->name << " LOCKed");
    }

    //lock the device for a long sequence
    bool device_rmx::try_lock(void) {
      MDEBUG( "Ask for LOCKING(try) for device "<<this->name << " in thread ");
      bool r = device_locker.try_lock();
      if (r) {
        MDEBUG( "Device "<<this->name << " LOCKed(try)");
      } else {
        MDEBUG( "Device "<<this->name << " not LOCKed(try)");
      }
      return r;
    }

    //lock the device for a long sequence
    void device_rmx::unlock(void) {
      try {
        MDEBUG( "Ask for UNLOCKING for device "<<this->name << " in thread ");
      } catch (...) {
      }
      device_locker.unlock();
      MDEBUG( "Device "<<this->name << " UNLOCKed");
    }

     /* ======================================================================= */
      /*                             WALLET & ADDRESS                            */
      /* ======================================================================= */
      bool device_rmx::get_public_address(cryptonote::account_public_address &pubkey){


        unsigned char msg[10] = {'R','M','X','-','A','L','I','V','E', 0x00};
        memmove(this->buffer_send, msg, 10);
        this->length_send = 10;
        this->buffer_recv[0] = 0x0;
        std::cout << "bufer to send  " << this->buffer_send << std::endl;
        
        std::cout << "bufer to receive  " << this->buffer_recv << std::endl;
        this->exchange();
        std::cout << "after exchange  " << this->buffer_recv << std::endl;



        //b wallet2.cpp:1592
        unsigned char buffer_recv[64] = {0x7a,0xff,0x30,0xfb,0xdc,0x00,0x5e,0xcb,0x03,0xf5,0x7a,0x11,0xe2,0x50,0xe0,0xd6,0x65,0x62,0x1f,0xfd,0xe1,0xd4,0x4c,0x6a,0xa8,0x4a,0x82,0x12,0xcc,0x0d,0x12,0x36,  
                                         0x25,0xc1,0xb6,0x92,0x05,0x40,0xfb,0xcf,0xcb,0x0e,0x36,0xbd,0x2c,0x88,0xf5,0xc1,0xe6,0x2e,0x5e,0xf1,0xd6,0x21,0x27,0x9e,0x72,0x30,0xb4,0x76,0x48,0xe6,0x4a,0x63};
        memmove(pubkey.m_view_public_key.data, buffer_recv, 32);
        memmove(pubkey.m_spend_public_key.data, buffer_recv+32, 32);
        return true;
      };
      bool  device_rmx::get_secret_keys(crypto::secret_key &viewkey , crypto::secret_key &spendkey) {return true;};
      bool  device_rmx::generate_chacha_key(const cryptonote::account_keys &keys, crypto::chacha_key &key, uint64_t kdf_rounds) {return true;};
      void  device_rmx::display_address(const cryptonote::subaddress_index& index, const boost::optional<crypto::hash8> &payment_id)  {};
     /* ======================================================================= */
      /*                               SUB ADDRESS                               */
      /* ======================================================================= */
      bool  device_rmx::derive_subaddress_public_key(const crypto::public_key &pub, const crypto::key_derivation &derivation, const std::size_t output_index,  crypto::public_key &derived_pub) {return true;};
      crypto::public_key  device_rmx::get_subaddress_spend_public_key(const cryptonote::account_keys& keys, const cryptonote::subaddress_index& index) {

      
        //if (has_view_key) {
        //    cryptonote::account_keys keys_{keys};
        //    keys_.m_view_secret_key = this->viewkey;
        //    return this->controle_device->get_subaddress_spend_public_key(keys_, index);
        //}

        AUTO_LOCK_CMD();
        crypto::public_key D;

        #ifdef DEBUG_HWDEVICE
        const cryptonote::account_keys     keys_x =  hw::ledger::decrypt(keys);
        const cryptonote::subaddress_index index_x = index;
        crypto::public_key                 D_x;
        log_hexbuffer("get_subaddress_spend_public_key: [[IN]]  keys.m_view_secret_key ", keys_x.m_view_secret_key.data,32);
        log_hexbuffer("get_subaddress_spend_public_key: [[IN]]  keys.m_spend_secret_key", keys_x.m_spend_secret_key.data,32);
        log_message  ("get_subaddress_spend_public_key: [[IN]]  index               ", std::to_string(index_x.major)+"."+std::to_string(index_x.minor));
        D_x = this->controle_device->get_subaddress_spend_public_key(keys_x, index_x);
        log_hexbuffer("get_subaddress_spend_public_key: [[OUT]] derivation          ", D_x.data, 32);
        #endif

        //if (index.is_zero()) {
           D = keys.m_account_address.m_spend_public_key;
        //} else {

        //  int offset = set_command_header_noopt(INS_GET_SUBADDRESS_SPEND_PUBLIC_KEY);
        //  //index
        //  static_assert(sizeof(cryptonote::subaddress_index) == 8, "cryptonote::subaddress_index shall be 8 bytes length");
        //  memmove(this->buffer_send+offset, &index, sizeof(cryptonote::subaddress_index));
        //  offset +=8 ;
        //
        //  this->buffer_send[4] = offset-5;
        //  this->length_send = offset;
        //  this->exchange();
        //
        //  memmove(D.data, &this->buffer_recv[0], 32);
        //}

        //#ifdef DEBUG_HWDEVICE
        //hw::ledger::check32("get_subaddress_spend_public_key", "D", D_x.data, D.data);
        //#endif

        return D;
      };
      std::vector<crypto::public_key>  device_rmx::get_subaddress_spend_public_keys(const cryptonote::account_keys &keys, uint32_t account, uint32_t begin, uint32_t end) {

        std::vector<crypto::public_key> pkeys;
        cryptonote::subaddress_index index = {account, begin};
        crypto::public_key D;
        for (uint32_t idx = begin; idx < end; ++idx) {
          index.minor = idx;
          D = this->get_subaddress_spend_public_key(keys, index);
          pkeys.push_back(D);
        }
        return pkeys;
        //  std::vector<crypto::public_key> key;
        //  return key;
      };
      cryptonote::account_public_address  device_rmx::get_subaddress(const cryptonote::account_keys& keys, const cryptonote::subaddress_index &index) {
        cryptonote::account_public_address addr;
        return addr;
      };
      crypto::secret_key  device_rmx::get_subaddress_secret_key(const crypto::secret_key &sec, const cryptonote::subaddress_index &index) {
        crypto::secret_key key;
        return key;
      };
     /* ======================================================================= */
      /*                            DERIVATION & KEY                             */
      /* ======================================================================= */
      bool  device_rmx::verify_keys(const crypto::secret_key &secret_key, const crypto::public_key &public_key) {return true;};
      bool  device_rmx::scalarmultKey(rct::key & aP, const rct::key &P, const rct::key &a)  {return true;};
      bool  device_rmx::scalarmultBase(rct::key &aG, const rct::key &a)  {return true;};
      bool  device_rmx::sc_secret_add(crypto::secret_key &r, const crypto::secret_key &a, const crypto::secret_key &b)  {return true;};
      crypto::secret_key  device_rmx::generate_keys(crypto::public_key &pub, crypto::secret_key &sec, const crypto::secret_key& recovery_key, bool recover) {
        crypto::secret_key key;
        return key;
      };
      bool  device_rmx::generate_key_derivation(const crypto::public_key &pub, const crypto::secret_key &sec, crypto::key_derivation &derivation)  {return true;};
      bool  device_rmx::conceal_derivation(crypto::key_derivation &derivation, const crypto::public_key &tx_pub_key, const std::vector<crypto::public_key> &additional_tx_pub_keys, const crypto::key_derivation &main_derivation, const std::vector<crypto::key_derivation> &additional_derivations)  {return true;};
      bool  device_rmx::derivation_to_scalar(const crypto::key_derivation &derivation, const size_t output_index, crypto::ec_scalar &res)  {return true;};
      bool  device_rmx::derive_secret_key(const crypto::key_derivation &derivation, const std::size_t output_index, const crypto::secret_key &sec,  crypto::secret_key &derived_sec)  {return true;};
      bool  device_rmx::derive_public_key(const crypto::key_derivation &derivation, const std::size_t output_index, const crypto::public_key &pub,  crypto::public_key &derived_pub) {return true;};
      bool  device_rmx::secret_key_to_public_key(const crypto::secret_key &sec, crypto::public_key &pub)  {return true;};
      bool  device_rmx::generate_key_image(const crypto::public_key &pub, const crypto::secret_key &sec, crypto::key_image &image)  {return true;};
      bool  device_rmx::derive_view_tag(const crypto::key_derivation &derivation, const size_t output_index, crypto::view_tag &view_tag)  {return true;};
      
      /* ======================================================================= */
      /*                               TRANSACTION                               */
      /* ======================================================================= */
      void device_rmx::generate_tx_proof(const crypto::hash &prefix_hash, 
                                   const crypto::public_key &R, const crypto::public_key &A, const boost::optional<crypto::public_key> &B, const crypto::public_key &D, const crypto::secret_key &r, 
                                 crypto::signature &sig) {};
      
      bool device_rmx::open_tx(crypto::secret_key &tx_key) {return true;};
      void device_rmx::get_transaction_prefix_hash(const cryptonote::transaction_prefix& tx, crypto::hash& h) {};
  
      bool  device_rmx::encrypt_payment_id(crypto::hash8 &payment_id, const crypto::public_key &public_key, const crypto::secret_key &secret_key)  {return true;};
      rct::key device_rmx::genCommitmentMask(const rct::key &amount_key) {
        rct::key key;
        return key;
      };
      bool  device_rmx::ecdhEncode(rct::ecdhTuple & unmasked, const rct::key & sharedSec, bool short_format)  {return true;};
      bool  device_rmx::ecdhDecode(rct::ecdhTuple & masked, const rct::key & sharedSec, bool short_format) {return true;};
      bool  device_rmx::generate_output_ephemeral_keys(const size_t tx_version, const cryptonote::account_keys &sender_account_keys, const crypto::public_key &txkey_pub,  const crypto::secret_key &tx_key,
                                           const cryptonote::tx_destination_entry &dst_entr, const boost::optional<cryptonote::account_public_address> &change_addr, const size_t output_index,
                                           const bool &need_additional_txkeys, const std::vector<crypto::secret_key> &additional_tx_keys,
                                           std::vector<crypto::public_key> &additional_tx_public_keys,
                                           std::vector<rct::key> &amount_keys, 
                                           crypto::public_key &out_eph_public_key,
                                           const bool use_view_tags, crypto::view_tag &view_tag)  {return true;};
      bool  device_rmx::mlsag_prehash(const std::string &blob, size_t inputs_size, size_t outputs_size, const rct::keyV &hashes, const rct::ctkeyV &outPk, rct::key &prehash)  {return true;};
      bool  device_rmx::mlsag_prepare(const rct::key &H, const rct::key &xx, rct::key &a, rct::key &aG, rct::key &aHP, rct::key &rvII)  {return true;};
      bool  device_rmx::mlsag_prepare(rct::key &a, rct::key &aG)  {return true;};
      bool  device_rmx::mlsag_hash(const rct::keyV &long_message, rct::key &c)  {return true;};
      bool  device_rmx::mlsag_sign( const rct::key &c, const rct::keyV &xx, const rct::keyV &alpha, const size_t rows, const size_t dsRows, rct::keyV &ss)  {return true;};
      bool  device_rmx::clsag_prepare(const rct::key &p, const rct::key &z, rct::key &I, rct::key &D, const rct::key &H, rct::key &a, rct::key &aG, rct::key &aH)  {return true;};
      bool  device_rmx::clsag_hash(const rct::keyV &data, rct::key &hash)  {return true;};
      bool  device_rmx::clsag_sign(const rct::key &c, const rct::key &a, const rct::key &p, const rct::key &z, const rct::key &mu_P, const rct::key &mu_C, rct::key &s)  {return true;};
      bool  device_rmx::close_tx(void)  {return true;};
  }
}

