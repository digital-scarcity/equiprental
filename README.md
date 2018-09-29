

# Open Questions  / TODO

# To build Docker
docker-compose build

# To run containers
docker-compose up -d

# To unlock wallet (use your password)
docker exec -it equiprental_nodeosd_1 /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900 wallet unlock --password PW5K6tHhBdjD7JGK4jwJKNnduamH3GfwmgJjnkmGvSPokKtanDrt3

# To view live logs
docker logs -f equiprental_nodeosd_1

# Setup the following aliases
alias cleos='docker exec -it equiprental_nodeosd_1 /opt/eosio/bin/cleos -u http://nodeosd:8888 --wallet-url http://keosd:8900'
alias eosiocpp='docker exec -it equiprental_nodeosd_1 eosiocpp'

# Here is the command to compile
eosiocpp -g /eosdev/equiprental/equiprental.abi /eosdev/equiprental/equiprental.hpp && eosiocpp -o /eosdev/equiprental/equiprental.wast /eosdev/equiprental/equiprental.cpp

