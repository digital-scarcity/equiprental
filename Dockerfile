#----------------------------
# EVA-EOS-DEV-Base
# EvaCoop base image of EVA-EOS-DEV
#----------------------------
# VERSION 1.1.0
# AUTHOR: Merouane Benthameur
# DESCRIPTION: base image of EVA-EOS-DEV

FROM eosio/eos-dev:latest
#MAINTAINER Merouane Benthameur <merouane.benthameur@eva.coop>
#LABEL authors="merouane.benthameur@eva.coop"

#COPY ./custom_config.ini /config.ini
#COPY ./eva-contracts-entrypoint.sh /
# COPY ./eva-contracts /opt/eosio/bin/data-dir/eva-contracts

#RUN chmod +x /eva-contracts-entrypoint.sh
