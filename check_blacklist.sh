#!/bin/bash

default_api="https://api1.eosasia.one"
default_config="./config.ini"
api=${default_api}
config=${default_config}

init() {
    [ "$1" != "" ] && api=$1
    [ "$2" != "" ] && config=$2
    [ ! -f ${config} ] && echo "please check config path(${config})" && exit 1
}

get_chain_actor_list() {
    url="${api}/v1/chain/get_table_rows"
    chain_actor_list=`curl -sX POST "${url}" \
     -d '{"scope":"theblacklist", "code":"theblacklist", "table":"theblacklist", "json": true, "limit": 100}' \
     | egrep -o 'accounts":\[[^{}]*\]'| egrep -o '"(\w*)"'| egrep -o '[^"]+' | sort -u`
    [ "${chain_actor_list}" == "" ] && echo "please check api(${api})." && exit 1
}

get_local_actor_list() {
    local_actor_list=`cat ${config} | grep actor-black | grep -v "#" |egrep -o '\w+$'| sort`
}

check_diff() {
    diff <(echo "${chain_actor_list}") <(echo "${local_actor_list}")| sed 's/</chain -/g' | sed 's/>/local -/g' | egrep 'chain -|local -'
}

# check local and theblacklist actor-blacklist hash
check_hash() {
    local_hash=`cat ${config} | grep actor-black | grep -v "#" | sort | tr -d " " | sha256sum`
    # get hash from table theblacklist
    chain_hash=`echo "${chain_actor_list}" | sed 's/^/actor-blacklist = /g' | tr -d " " | sha256sum`
    if [ "${local_hash}" == "${chain_hash}" ];then
        echo "success: ${chain_hash}"
    else
        echo "local: ${local_hash}"
        echo "chain: ${chain_hash}"
    fi
}

main() {
    init $@
    get_chain_actor_list
    get_local_actor_list
    check_diff
    check_hash
}

main $@