--TEST--
Cookie samesite
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) die "skip"; ?>
--INI--
sp.configuration_file={PWD}/config/config_samesite_cookies.ini
--COOKIE--
super_cookie=if_there_is_no_cookie_here_there_is_no_header_list
--ENV--
return <<<EOF
REMOTE_ADDR=2001:0db8:0000:0000:0000:fe00:0042:8329
HTTP_USER_AGENT=Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/59.0.3071.109 Chrome/59.0.3071.109 Safari/537.36
HTTPS=1
EOF;
--FILE--
<?php
setcookie("super_cookie", "super_value");
setcookie("awful_cookie", "awful_value");
setcookie("not_encrypted", "test_value", 1, "1", "1", false, true);
setcookie("nice_cookie", "nice_value", 1, "1", "1", true, true);

$expected = array(
    'Set-Cookie: super_cookie=super_value; path=; samesite=Lax',
    'Set-Cookie: awful_cookie=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFyZcYjfEskB0AU0V3%2BvwazcRuU%2Ft6KpcUahvxw%3D; path=; samesite=Strict; HttpOnly',
    'Set-Cookie: not_encrypted=test_value; expires=Thu, 01-Jan-1970 00:00:01 GMT; Max-Age=0; path=1; domain=1; HttpOnly',
    'Set-Cookie: nice_cookie=AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJ8ko%2ByA4y%2Bmw5MGBx8fgc3TWOAvhIu%2BfF%2Bx2g%3D%3D; expires=Thu, 01-Jan-1970 00:00:01 GMT; Max-Age=0; path=1; samesite=Strict; domain=1; secure; HttpOnly',
    );

$headers = headers_list();
if (($i = count($expected)) > count($headers))
{
  echo "Fewer headers are being sent than expected - aborting";
  return;
}

do
{
  if (strncmp(current($headers), 'Set-Cookie:', 11) !== 0)
  {
    continue;
  }

  if (current($headers) === current($expected))
  {
    $i--;
  }
  else
  {
    echo "Header mismatch:\n\tExpected: "
      .current($expected)
      ."\n\tReceived: ".current($headers)."\n";
  }

  next($expected);
}
while (next($headers) !== FALSE);

echo ($i === 0)
  ? 'OK'
  : 'A total of '.$i.' errors found.';
?>
--EXPECT--
OK
