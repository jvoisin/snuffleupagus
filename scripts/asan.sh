#!/bin/sh

php_path="$(which php)"
php_path="/home/travis/.phpenv/versions/7.0.22/bin/php"
#libasan_path="/usr/lib/x86_64-linux-gnu/libasan.so.3"
libasan_path="/usr/lib/gcc/x86_64-linux-gnu/4.8/libasan.so"
script_name="./preload_script"

_php_path=$(echo ${php_path} | sed 's/\//\\\//g')
_script_name=$(echo ${script_name} | sed 's/\//\\\//g')

[ -f ${php_path} ] || (echo "${php_path} doesn't exist"; exit 1)
[ -f ${libasan_path} ] || (echo "${libasan_path} doesn't exist"; exit 1)

cd ../src

echo "#!/bin/sh
LD_PRELOAD=\"${libasan_path}\" ${php_path} \"\$@\"" >  "${script_name}"
chmod +x "${script_name}"

phpize || exit
CFLAGS="-fsanitize=address -fno-omit-frame-pointer -g -ggdb" CC="gcc" ./configure --enable-debug --enable-snuffleupagus  || (exit 1)
sed -i -e "s/${_php_path}/${_script_name}/g" Makefile
make clean
NO_INTERACTION=1 REPORT_EXIT_STATUS=1 make test

rm "${script_name}"
