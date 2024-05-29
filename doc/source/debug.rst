:orphan:

.. This page is included in the FAQ, but is not present in any ToC,
   because its content is pretty technical, and "normal users" shouldn't
   need to read it.

..  _debug:

Debugging crashes
=================

..  _testsuite_fail:

The testsuite is failing
-------------------------

We're using `php qa <https://qa.php.net/>`__ tests format for our testsuite,
it is automatically run when you're building snuffleupagus.

If it happens to have unexpected failures (Since we're using `TDD <https://en.wikipedia.org/wiki/Test-driven_development>`__ as much
as we can, we do have some expected failures), please do `open an issue <https://github.com/jvoisin/snuffleupagus/issues/new>`__
on our bugtracker, and attach the generated ``.diff`` and ``.out`` files to it,
so we can see what's happening.

Snuffleupagus is crashing
-------------------------

While we do our very best to make snuffleupagus solid as possible, we're humans,
and computers are hard, so crashes can happen. If you're encountering one in production,
please try to launch the `testsuite <https://github.com/jvoisin/snuffleupagus/blob/master/CONTRIBUTING.md#3-get-the-test-suite-running>`__
to see if it's failing. If it does, please :ref:`tell us <testsuite_fail>`.

If the testsuite is passing, odds are that you're encountering an issue tied to your php code,
so unless you're able to provide it to us, or are willing to give us a shell on your infrastructure,
you'll have to gather information on your own. Don't be afraid, it's not that hard.

The first step is to build snuffleupagus with debug symbols, with ``make debug``.

Snuffleupagus is crashing at startup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you can can reproduce the crash with ``php -d "sp.configuration_file=/path/to/your/config/config.rules" -d "extension=/complete/path/to/debug/snuffleupagus.so"``,
please run it in ``gdb``, with ``gdb --args /usr/bin/php -d "sp.configuration_file=/path/to/your/config/config.rules" -d "extension=/complete/path/to/debug/snuffleupagus.so"``.

In ``gdb``, type ``run`` to *run*, the crash should now appear. Type ``bt`` to get a complete backtrace,
and ``info registers`` to get the content of the registers. Please provide
us the complete output of gdb in the issue. It should look like this:

::

  $ gdb --args /usr/bin/php7.0 -d "sp.configuration_file=/config_disabled_functions_param_alias.ini" -d "extension=/home/jvoisin/snuffleupagus/src/modules/snuffleupagus.so"
  (gdb) r
  Starting program: /usr/bin/php7.0 -d sp.configuration_file=/config_disabled_functions_param_alias.ini -d extension=/home/jvoisin/snuffleupagus/src/modules/snuffleupagus.so
  
  Program received signal SIGSEGV, Segmentation fault.
  0x00007fffe25558f5 in parse_str (line=0x7fffddd3602d "\"system\").alias(\"1\").drop();", keyword=0x7fffe25673ec ".function(", retval=0x7fffddd33028) at /home/jvoisin/snuffleupagus/src/sp_config.c:107
  107		char a = *x;
  (gdb) bt
  #0  0x00007fffe25558f5 in parse_str (line=0x7fffddd3602d "\"system\").alias(\"1\").drop();", keyword=0x7fffe25673ec ".function(", retval=0x7fffddd33028) at /home/jvoisin/snuffleupagus/src/sp_config.c:107
  #1  0x00007fffe2555e2c in parse_keywords (funcs=0x7fffffffc700, line=0x7fffddd3602d "\"system\").alias(\"1\").drop();") at /home/jvoisin/snuffleupagus/src/sp_config_utils.c:11
  #2  0x00007fffe2562ff6 in parse_disabled_functions (line=0x7fffddd36023 ".function(\"system\").alias(\"1\").drop();") at /home/jvoisin/snuffleupagus/src/sp_config_keywords.c:219
  #3  0x00007fffe2555609 in parse_line (line=0x7fffddd36010 "sp.disable_function.function(\"system\").alias(\"1\").drop();") at /home/jvoisin/snuffleupagus/src/sp_config.c:46
  #4  0x00007fffe2555bf5 in sp_parse_config (conf_file=0x7ffff7e14028 "/config_disabled_functions_param_alias.ini") at /home/jvoisin/snuffleupagus/src/sp_config.c:182
  #5  0x00007fffe2555331 in OnUpdateConfiguration (entry=0x7fffddd39010, new_value=0x7ffff7e14010, mh_arg1=0x0, mh_arg2=0x0, mh_arg3=0x0, stage=1) at /home/jvoisin/snuffleupagus/src/snuffleupagus.c:176
  #6  0x00005555557d2861 in zend_register_ini_entries ()
  #7  0x00007fffe2554fc9 in zm_startup_snuffleupagus (type=1, module_number=53) at /home/jvoisin/snuffleupagus/src/snuffleupagus.c:92
  #8  0x00005555557bb9ae in zend_startup_module_ex ()
  #9  0x00005555557bba5c in ?? ()
  #10 0x00005555557c902a in zend_hash_apply ()
  #11 0x00005555557bbd1a in zend_startup_modules ()
  #12 0x00005555557555db in php_module_startup ()
  #13 0x000055555584d02d in ?? ()
  #14 0x0000555555638581 in main ()
  (gdb) info registers
  rax            0x0	0
  rbx            0x7fffe2562acc	140736990685900
  rcx            0x7fffe25558cd	140736990632141
  rdx            0x7fffddd33028	140736914993192
  rsi            0x7fffe25673ec	140736990704620
  rdi            0x7fffddd3602d	140736915005485
  rbp            0x7fffffffc660	0x7fffffffc660
  rsp            0x7fffffffc620	0x7fffffffc620
  r8             0xffff	65535
  r9             0x1	1
  r10            0x7ffff653b2a0	140737326068384
  r11            0x0	0
  r12            0x7ffff3b7cff0	140737282297840
  r13            0x35	53
  r14            0x15	21
  r15            0x7fffe2565525	140736990696741
  rip            0x7fffe25558f5	0x7fffe25558f5 <parse_str+40>
  eflags         0x10202	[ IF RF ]
  cs             0x33	51
  ss             0x2b	43
  ds             0x0	0
  es             0x0	0
  fs             0x0	0
  gs             0x0	0
  (gdb) 
  
    
  
Snuffleupagus is crashing at runtime
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If snuffleupagus is crashing at runtime, odds are that it's triggered by a request,
and that this behaviour is tied to your php code. The simplest way to gather information
about what's going on is to generate a `core dump <https://en.wikipedia.org/wiki/Core_dump>`__,
as written in the `php documentation <https://bugs.php.net/bugs-generating-backtrace.php>`__:

You need to:

1. Enable them with ``echo 0 > /proc/sys/kernel/core_uses_pid``
2. Set a pattern for them with ``echo '/tmp/core-%e.%p' > /proc/sys/kernel/core_pattern``,
3. Remove the maximum size of a dump ``ulimit -c unlimited``

You're now ready to restart your PHP stack, using the *debug* version of Snuffleupagus,
and right after the crash, you should get a (big) file in your ``/tmp``, starting with ``core``:
It's a complete memory snapshot of the state of the process during the crash.
You can either send it to us (along with your ``snufflepagus.so`` binary),
or if you're not comfortable with giving us a complete dump of your php memory,
run ``gdb /usr/bin/php /tmp/core-php-fpm-1337``, then ``bt`` to get a backtrace, and ``info registers`` to get the registers.
