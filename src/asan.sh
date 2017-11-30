#!/bin/sh

php_path=$(which php)
# libasan_path=$(find / -name 'libasan.so' 2>/dev/null | head -n 1)
libasan_path="/usr/lib/gcc/x86_64-linux-gnu/4.9/libasan.so"
script_name="./preload_script"

_php_path=$(echo ${php_path} | sed 's/\//\\\//g')
_script_name=$(echo ${script_name} | sed 's/\//\\\//g')

echo "#!/bin/sh
LD_PRELOAD=\"${libasan_path}\" ${php_path} \"\$@\"" >  "${script_name}"
chmod +x "${script_name}"

phpize || exit
CFLAGS="-fsanitize=address -fno-omit-frame-pointer -g -ggdb" CC="gcc" ./configure --enable-debug --enable-snuffleupagus  || exit
sed -i -e "s/${_php_path}/${_script_name}/g" Makefile
make clean
NO_INTERACTION=1 REPORT_EXIT_STATUS=1 make test

rm "${script_name}"
