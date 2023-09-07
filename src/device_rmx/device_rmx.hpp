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


#pragma once

#include <cstddef>
#include <string>
#include "device/device.hpp"
#include "device/log.hpp"
#include "device/device_io_serial.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace hw {

    namespace rmx {

    /* Minimal supported version */
    #define MINIMAL_APP_VERSION_MAJOR    1
    #define MINIMAL_APP_VERSION_MINOR    8
    #define MINIMAL_APP_VERSION_MICRO    0

  

    #define BUFFER_SEND_SIZE 262
    #define BUFFER_RECV_SIZE 262


    void register_all();//std::map<std::string, std::unique_ptr<device>> &registry);

    class device_rmx : public hw::device {
      private:
        // Locker for concurrent access
        mutable boost::recursive_mutex   device_locker;
        mutable boost::mutex   command_locker;

        //IO
        hw::io::device_io_serial hw_device;
        unsigned int  length_send;
        unsigned char buffer_send[262];
        unsigned int  length_recv;
        unsigned char buffer_recv[262];
        unsigned int  sw;


        unsigned int exchange(unsigned int ok=0x9000, unsigned int mask=0xFFFF);

        // To speed up blockchain parsing the view key maybe handle here.
        crypto::secret_key viewkey;
        bool has_view_key;

        device *control_device_here;


        
      public:
        device_rmx();
        ~device_rmx();
        
        



      /////////////   vsechno tohle tady musi byt deklarovane:
      explicit operator bool() const override {return true;}//this->connected(); }
      device_type get_type() const override {return device_type::RMX;};

      /* ======================================================================= */
      /*                              SETUP/TEARDOWN                             */
      /* ======================================================================= */
      bool set_name(const std::string &name) override;

      const std::string get_name() const override;
      bool init() override;
      bool release() override;
      bool connect() override;
      bool disconnect() override;
      bool connected(void);

      /* ======================================================================= */
      /*  LOCKER                                                                 */
      /* ======================================================================= */ 
      void lock(void)  override;
      void unlock(void) override;
      bool try_lock(void) override;
     /* ======================================================================= */
      /*                             WALLET & ADDRESS                            */
      /* ======================================================================= */
      bool  get_public_address(cryptonote::account_public_address &pubkey) override;
      bool  get_secret_keys(crypto::secret_key &viewkey , crypto::secret_key &spendkey) override;
      bool  generate_chacha_key(const cryptonote::account_keys &keys, crypto::chacha_key &key, uint64_t kdf_rounds) override;
      void  display_address(const cryptonote::subaddress_index& index, const boost::optional<crypto::hash8> &payment_id) override;
     /* ======================================================================= */
      /*                               SUB ADDRESS                               */
      /* ======================================================================= */
      bool  derive_subaddress_public_key(const crypto::public_key &pub, const crypto::key_derivation &derivation, const std::size_t output_index,  crypto::public_key &derived_pub) override;
      crypto::public_key  get_subaddress_spend_public_key(const cryptonote::account_keys& keys, const cryptonote::subaddress_index& index) override;
      std::vector<crypto::public_key>  get_subaddress_spend_public_keys(const cryptonote::account_keys &keys, uint32_t account, uint32_t begin, uint32_t end) override;
      cryptonote::account_public_address  get_subaddress(const cryptonote::account_keys& keys, const cryptonote::subaddress_index &index) override;
      crypto::secret_key  get_subaddress_secret_key(const crypto::secret_key &sec, const cryptonote::subaddress_index &index) override;
     /* ======================================================================= */
      /*                            DERIVATION & KEY                             */
      /* ======================================================================= */
      bool  verify_keys(const crypto::secret_key &secret_key, const crypto::public_key &public_key) override;
      bool  scalarmultKey(rct::key & aP, const rct::key &P, const rct::key &a) override;
      bool  scalarmultBase(rct::key &aG, const rct::key &a) override;
      bool  sc_secret_add(crypto::secret_key &r, const crypto::secret_key &a, const crypto::secret_key &b) override;
      crypto::secret_key  generate_keys(crypto::public_key &pub, crypto::secret_key &sec, const crypto::secret_key& recovery_key = crypto::secret_key(), bool recover = false) override;
      bool  generate_key_derivation(const crypto::public_key &pub, const crypto::secret_key &sec, crypto::key_derivation &derivation) override;
      bool  conceal_derivation(crypto::key_derivation &derivation, const crypto::public_key &tx_pub_key, const std::vector<crypto::public_key> &additional_tx_pub_keys, const crypto::key_derivation &main_derivation, const std::vector<crypto::key_derivation> &additional_derivations) override;
      bool  derivation_to_scalar(const crypto::key_derivation &derivation, const size_t output_index, crypto::ec_scalar &res) override;
      bool  derive_secret_key(const crypto::key_derivation &derivation, const std::size_t output_index, const crypto::secret_key &sec,  crypto::secret_key &derived_sec) override;
      bool  derive_public_key(const crypto::key_derivation &derivation, const std::size_t output_index, const crypto::public_key &pub,  crypto::public_key &derived_pub) override;
      bool  secret_key_to_public_key(const crypto::secret_key &sec, crypto::public_key &pub) override;
      bool  generate_key_image(const crypto::public_key &pub, const crypto::secret_key &sec, crypto::key_image &image) override;
      bool  derive_view_tag(const crypto::key_derivation &derivation, const size_t output_index, crypto::view_tag &view_tag) override;
      
      /* ======================================================================= */
      /*                               TRANSACTION                               */
      /* ======================================================================= */
      void generate_tx_proof(const crypto::hash &prefix_hash, 
                                   const crypto::public_key &R, const crypto::public_key &A, const boost::optional<crypto::public_key> &B, const crypto::public_key &D, const crypto::secret_key &r, 
                                 crypto::signature &sig) override;
      
      bool  open_tx(crypto::secret_key &tx_key) override;
      void get_transaction_prefix_hash(const cryptonote::transaction_prefix& tx, crypto::hash& h) override;
  
      bool  encrypt_payment_id(crypto::hash8 &payment_id, const crypto::public_key &public_key, const crypto::secret_key &secret_key) override;
      rct::key genCommitmentMask(const rct::key &amount_key) override;
      bool  ecdhEncode(rct::ecdhTuple & unmasked, const rct::key & sharedSec, bool short_format) override;
      bool  ecdhDecode(rct::ecdhTuple & masked, const rct::key & sharedSec, bool short_format) override;
      bool  generate_output_ephemeral_keys(const size_t tx_version, const cryptonote::account_keys &sender_account_keys, const crypto::public_key &txkey_pub,  const crypto::secret_key &tx_key,
                                           const cryptonote::tx_destination_entry &dst_entr, const boost::optional<cryptonote::account_public_address> &change_addr, const size_t output_index,
                                           const bool &need_additional_txkeys, const std::vector<crypto::secret_key> &additional_tx_keys,
                                           std::vector<crypto::public_key> &additional_tx_public_keys,
                                           std::vector<rct::key> &amount_keys, 
                                           crypto::public_key &out_eph_public_key,
                                           const bool use_view_tags, crypto::view_tag &view_tag) override;
      bool  mlsag_prehash(const std::string &blob, size_t inputs_size, size_t outputs_size, const rct::keyV &hashes, const rct::ctkeyV &outPk, rct::key &prehash) override;
      bool  mlsag_prepare(const rct::key &H, const rct::key &xx, rct::key &a, rct::key &aG, rct::key &aHP, rct::key &rvII) override;
      bool  mlsag_prepare(rct::key &a, rct::key &aG) override;
      bool  mlsag_hash(const rct::keyV &long_message, rct::key &c) override;
      bool  mlsag_sign( const rct::key &c, const rct::keyV &xx, const rct::keyV &alpha, const size_t rows, const size_t dsRows, rct::keyV &ss) override;
      bool clsag_prepare(const rct::key &p, const rct::key &z, rct::key &I, rct::key &D, const rct::key &H, rct::key &a, rct::key &aG, rct::key &aH) override;
      bool clsag_hash(const rct::keyV &data, rct::key &hash) override;
      bool clsag_sign(const rct::key &c, const rct::key &a, const rct::key &p, const rct::key &z, const rct::key &mu_P, const rct::key &mu_C, rct::key &s) override;
      bool  close_tx(void) override;
    };
    //#endif  //WITH_DEVICE_LEDGER
  };

};

