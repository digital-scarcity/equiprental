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
#include <functional>

using namespace eosio;
using namespace std;
using std::string;

class equiprental : public contract
{

  public:
    equiprental(account_name self) : contract(self) {}

    // @abi action
    void newequip ( const string        name,
                    const string        description,
                    const account_name  owner,
                    const account_name  token_contract,
                    const asset         hourly_rate);

    // @abi action
    void remequip (const uint64_t equipment_id);

    // @abi action
    void rent ( const uint64_t      equipment_id,
                const account_name  renter,
                const asset         payment);

    // @abi action
    void evict ( const uint64_t     rental_id);

    // @abi action
    void returnequip ( const uint64_t   rental_id );

    void apply(const account_name contract, const account_name act);

    void transferReceived(const currency::transfer &transfer, const account_name code);

  private:

    // @abi table balances i64
    struct balance {
        asset funds;
        account_name token_contract;
        uint64_t primary_key() const { return funds.symbol; }
    };

    typedef eosio::multi_index<N(balances), balance> balance_table;

    // @abi table equipments
    struct equipment
    {
        uint64_t        equipment_id;
        string          name;
        string          description;
        account_name    owner;
        account_name    token_contract;
        asset           hourly_rate;

        uint64_t primary_key() const { return equipment_id; }
   };

    typedef eosio::multi_index<N(equipments), equipment> equipment_table;

    // @abi table rentals
    struct rental {
        uint64_t        equipment_id;
        account_name    renter;
        uint32_t        start_rent;
        uint32_t        last_updated;
        uint32_t        expiration;

        uint64_t    primary_key() const { return equipment_id; }
    };

    typedef eosio::multi_index<N(rentals), rental> rental_table;

    void paytoken(  const account_name  token_contract,
                    const account_name from,
                    const account_name to,
                    const asset token_amount,
                    const string memo)
    {
        print("---------- Payment -----------\n");
        print("Token Contract   : ", name{token_contract}, "\n");
        print("From             : ", name{from}, "\n");
        print("To               : ", name{to}, "\n");
        print("Amount           : ", token_amount, "\n");
        print("Memo             : ", memo, "\n");

        action(
            permission_level{from, N(active)},
            token_contract, N(transfer),
            std::make_tuple(from, to, token_amount, memo))
            .send();

        print("---------- End Payment -------\n");
    }
  
};
