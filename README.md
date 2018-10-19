

# Purpose
This repo is designed to provide a step-by-step framework and example for creating a smart contract on EOS. It uses a simple use case of equipment rentals that list their equipment for rent and renters who pay rent for those items.

# Process Steps
Here are the general steps to developing a smart contract.
![alt text](https://github.com/digital-scarcity/equiprental/blob/master/img/steps.png "Steps")


## Data Model
Define the data that will be persisted in the smart contract
![alt text](https://github.com/digital-scarcity/equiprental/blob/master/img/datamodel.png "Data Model")

## Actions
Define the actions, or state changes, that must exist in your smart contract
![alt text](https://github.com/digital-scarcity/equiprental/blob/master/img/actions.png "Actions")

## Pre- and Post-Conditions
Define the actions, or state changes, that must exist in your smart contract
![alt text](https://github.com/digital-scarcity/equiprental/blob/master/img/prepostconditions.png "Pre- and Post-Conditions")

# Technical Steps

## To build Docker
You may want to update version inside the Dockerfile.
```
docker-compose build
docker volume create --name=nodeos-data-volume
docker volume create --name=keosd-data-volume
```

## Run containers and open new terminal to view logs
```
docker-compose up -d
```

I usually run in this in a separate terminal that I can view as I run tests.
```
docker logs -f equiprental_nodeosd_1
```

## To unlock wallet (use your password)
You must unlock your wallet and import private key.
```
docker exec -it equiprental_nodeosd_1 /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900 wallet unlock --password <<<>>

cleos wallet import --private-key "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"
```


## Setup the following aliases
```
alias cleos='docker exec -it equiprental_nodeosd_1 /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900'
alias eosiocpp='docker exec -it equiprental_nodeosd_1 eosiocpp'
```

## Bash commands to setup and execute actions 
```
# Compile smart contract
eosiocpp -g /eosdev/contracts/equiprental/equiprental.abi /eosdev/contracts/equiprental/equiprental.hpp && eosiocpp -o /eosdev/contracts/equiprental/equiprental.wast /eosdev/contracts/equiprental/equiprental.cpp

# Create some accounts for usage
cleos create account eosio equiprental EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio owner EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn
cleos create account eosio renter EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn

# Token is required for rental payments
cleos create account eosio token EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn EOS7ckzf4BMgxjgNSYV22rtTXga8R9Z4XWVhYp8TBgnBi2cErJ2hn

# Update authority so that equiprental@eosio.code can send tokens on behalf of equiprental
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

```