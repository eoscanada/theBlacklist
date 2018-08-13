#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosio.system/eosio.system.hpp>

using eosio::asset;
using std::string;

class theblacklist_contract : public eosio::contract {
 public:
  theblacklist_contract(account_name self) :
    eosio::contract(self),
    theblacklist(_self, _self) {}


  // A simple store for a producer's json.
  void set(const account_name owner, const std::vector<account_name>& accounts, const string order_name, const string action) {
    eosio::print(" | set action called");
    eosio::print(" | owner:", owner);
    eosio::print(" | order:", order_name);
    require_auth(owner);
    eosio::print(" | auth granted!");

    // Check producer info table. Owner should exist (has called regproducer) and be activated.
    /*
    typedef eosio::multi_index<N(producers), eosiosystem::producer_info> producer_info_t;
    producer_info_t _producers(N(eosio), N(eosio));
    //eosio::print("prods:", _producers);
    auto prod = _producers.get(owner,    "user is not a producer");
    eosio_assert(prod.is_active == true, "user is not an active producer");
    eosio_assert(prod.total_votes > 0.0, "user is not an active producer");
    */

    // Quick check to remind the user the payload must be json.
    // eosio_assert(json[0] == '{',             "payload must be json");
    // eosio_assert(json[json.size()-1] == '}', "payload must be json");

    // If entry exists, update it.
    auto target_itr = theblacklist.find(owner);
    if (target_itr != theblacklist.end()) {
      theblacklist.modify(target_itr, owner, [&](auto& j) {
        j.owner = owner;
        j.action = action;
        j.accounts = accounts;
        j.order_name = order_name;
      });
    } else {  // Otherwise, create a new entry for them.
      theblacklist.emplace(owner, [&](auto& j) {
        j.owner = owner;
        j.action = action;
        j.accounts = accounts;
        j.order_name = order_name;
      });
    }
  }


  // Allows a producer to delete their entry.
  void del(const account_name owner) {
    require_auth(owner);
    auto target_itr = theblacklist.find(owner);
    theblacklist.erase(target_itr);
  }


 private:
  // @abi table theblacklist i64
  struct theblacklist {
    account_name                owner;
    std::vector<account_name>   accounts;
    string                      order_name; // in ECAF Order 001, order_name should be string '2018-06-19-AO-001'.
    // checksum256                 order_tx; // transaction id contained ECAF Order signed by account ecafofficial.
    string                      action; // action is a choice field, valid choices are 'add' and 'remove', meaning add or remove accounts from blacklist. Default is 'add'.

    auto primary_key() const {  return owner;  }
    EOSLIB_SERIALIZE(theblacklist, (owner)(accounts)(order_name)(action))
  };
  typedef eosio::multi_index<N(theblacklist), theblacklist> theblacklist_table;
  theblacklist_table theblacklist;
};

EOSIO_ABI(theblacklist_contract, (set)(del))
