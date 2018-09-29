
eosiocpp -g /eosdev/contracts/equiprental/equiprental.abi /eosdev/contracts/equiprental/equiprental.hpp && eosiocpp -o /eosdev/contracts/equiprental/equiprental.wast /eosdev/contracts/equiprental/equiprental.cpp

cleos create account eosio equiprental EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio owner EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio renter EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio renter2 EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio token EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn

cleos push action eosio updateauth '{"account":"equiprental","permission":"active","parent":"owner","auth":{"keys":[{"key":"EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn", "weight":1}],"threshold":1,"accounts":[{"permission":{"actor":"equiprental","permission":"eosio.code"},"weight":1}],"waits":[]}}' -p equiprental@owner

cleos set contract equiprental /eosdev/contracts/equiprental
cleos set contract token /eosdev/contracts/eosio.token

cleos push action token create '["token", "1000000.00 RENT"]' -p token
cleos push action token issue '["renter", "1000.00 RENT", "memo"]' -p token
cleos push action token issue '["renter2", "1000.00 RENT", "memo"]' -p token

cleos push action equiprental newequip '["Riding Lawn Mower", "Used for mowing grass, cuts at 2.5 MPH", "owner", "token", "0.05 RENT"]' -p owner
cleos push action equiprental newequip '["Table Saw", "Cut wood, not fingers", "owner", "token", "1.0 RENT"]' -p owner
cleos push action equiprental newequip '["EOS Account - favoriteacct", "Has access to some cool stuff to see", "owner", "token", "20.00 RENT"]' -p owner

cleos push action token transfer '["renter", "equiprental", "1.00 RENT", "initial deposit"]' -p renter

cleos get table equiprental renter balances

cleos push action equiprental rent '[0, "renter", "0.1 RENT"]' -p renter

cleos get table equiprental equiprental equipments

cleos get table token owner accounts


#######  Test Eviction and Equipment Returns

cleos push action equiprental rent '[1, "renter", "0.05 RENT"]' -p renter

cleos get table equiprental equiprental rentals

cleos push action equiprental evict '[1]' -p owner

cleos push action equiprental returnequip '[2]' -p renter

