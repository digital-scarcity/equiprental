#include <equiprental.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/currency.hpp>

#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/types.hpp>
#include <eosiolib/action.hpp>
#include <eosiolib/symbol.hpp>
#include <eosiolib/crypto.h>
#include <cstring>
#include <math.h>

using namespace std;
using namespace eosio;

void equiprental::newequip (const string        name,
                            const string        description,
                            const account_name  owner,
                            const account_name  token_contract,
                            const asset         hourly_rate) {

    equipment_table e_t (_self, _self);
    e_t.emplace (owner, [&](auto &e) {
        e.equipment_id = e_t.available_primary_key();
        e.name          = name;
        e.description   = description;
        e.owner         = owner;
        e.token_contract    = token_contract;
        e.hourly_rate   = hourly_rate;
    });
}

void equiprental::remequip (const uint64_t equipment_id) {
    equipment_table e_t (_self, _self);
    auto e_itr = e_t.find (equipment_id);
    eosio_assert (e_itr != e_t.end(), "Equipment ID not found.");

    require_auth (e_itr->owner);

    e_t.erase (e_itr);
}

void equiprental::rent ( const uint64_t     equipment_id,
                         const account_name renter, 
                         const asset        payment ) {

    equipment_table e_t (_self, _self);
    auto e_itr = e_t.find (equipment_id);
    eosio_assert (e_itr != e_t.end(), "Equipment ID not found.");
 
    balance_table balances(_self, renter);
    auto b_itr = balances.find(e_itr->hourly_rate.symbol);
    eosio_assert (b_itr != balances.end(), "No balance for required symbol found.");
    
    print(name{renter}, " has ", b_itr->funds, " available\n");
    
    rental_table r_t (_self, _self);
    auto r_itr = r_t.find (equipment_id);
    
    if (r_itr == r_t.end()) {
        r_t.emplace (renter, [&](auto &r) {
            r.equipment_id   = equipment_id;
            r.expiration    = now () + (e_itr->hourly_rate.amount * b_itr->funds.amount * 60);
            r.start_rent    = now();
            r.last_updated  = now();
            r.renter        = renter;
        });
    } else {
        eosio_assert (r_itr->renter == renter, "Equipment is already rented.");
        r_t.modify (r_itr, renter, [&](auto &r) {
            r.expiration    = r_itr->expiration + (e_itr->hourly_rate.amount * b_itr->funds.amount * 60);
            r.last_updated  = now();
            r.renter        = renter;
        });
    }
    
    paytoken (e_itr->token_contract, _self, e_itr->owner, b_itr->funds, "Rental : " + to_string(equipment_id));
    
    balances.erase (b_itr);
}

void equiprental::evict (const uint64_t rental_id) {
    rental_table r_t (_self, _self);
    auto r_itr = r_t.find (rental_id);
    eosio_assert (r_itr != r_t.end(), "Rental ID not found.");
    eosio_assert (r_itr->expiration < now(), "Rental period has not yet expired.");

    equipment_table e_t (_self, _self);
    auto e_itr = e_t.find (rental_id);
    eosio_assert (e_itr != e_t.end(), "Equipment ID not found.");
 
    require_auth (e_itr->owner);

    r_t.erase (r_itr);
}

void equiprental::returnequip (const uint64_t rental_id) {
    rental_table r_t (_self, _self);
    auto r_itr = r_t.find (rental_id);
    eosio_assert (r_itr != r_t.end(), "Rental ID not found.");

    require_auth (r_itr->renter);
    
    r_t.erase (r_itr);
}


void equiprental::apply(const account_name contract, const account_name act)
{
    if (act == N(transfer))
    {
        transferReceived(unpack_action_data<currency::transfer>(), contract);
        return;
    }

    auto &thiscontract = *this;

    switch (act)
    {
        EOSIO_API(equiprental, (newequip)(remequip)(rent)(evict)(returnequip))
    };
}

void equiprental::transferReceived(const currency::transfer &transfer, const account_name code)
{
    if (transfer.to != _self)
    {
        return;
    }
              
    print("Account Name     :   ", name{code}, "\n");
    print("From             :   ", name{transfer.from}, "\n");
    print("To               :   ", name{transfer.to}, "\n");
    print("Asset            :   ", transfer.quantity, "\n");
    print("Received Amount  :   ", transfer.quantity.amount, "\n");
    print("Received Symbol  :   ", transfer.quantity.symbol, "\n");
    print("Memo             :   ", transfer.memo, "\n");
    
    balance_table balances(_self, transfer.from);
    asset new_balance;
    auto it = balances.find(transfer.quantity.symbol);
    if(it != balances.end()) {
        eosio_assert (it->token_contract == code, "Transfer does not match existing token contract.");
        balances.modify(it, transfer.from, [&](auto& bal){
            // Assumption: total currency issued by eosio.token will not overflow asset
            bal.funds += transfer.quantity;
            new_balance = bal.funds;
        });
    }
    else
        balances.emplace(transfer.from, [&](auto& bal){
            bal.funds = transfer.quantity;
            bal.token_contract  = code;
            new_balance = transfer.quantity;
        });

    print(name{transfer.from}, " deposited:       ", transfer.quantity, "\n");
    print(name{transfer.from}, " funds available: ", new_balance);
}


extern "C"
{
    using namespace eosio;

    void apply(uint64_t receiver, uint64_t code, uint64_t action)
    {
        auto self = receiver;
        equiprental contract(self);
        contract.apply(code, action);
        eosio_exit(0);
    }
}
