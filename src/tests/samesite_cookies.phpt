--TEST--
Cookie samesite
--SKIPIF--
<?php
if (!extension_loaded("snuffleupagus")) die("skip");
?>
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
if (!setcookie("super_cookie", "super_value")) {
  echo "setcookie failed.\n";
}
if (!setcookie("awful_cookie", "awful_value")) {
  echo "setcookie failed.\n";
}
if (!setcookie("not_encrypted", "test_value", 1, "1", "1", false, true)) {
  echo "setcookie failed.\n";
}
if (!setcookie("nice_cookie", "nice_value", 1, "1", "1", true, true)) {
  echo "setcookie failed.\n";
}

// If the cookie value start with "!", it means that we don't want the value in the headers, but the encrypted cookie
$expected = array(
  "awful_cookie" => "!awful_value",
  "not_encrypted" => "test_value",
  "nice_cookie" => "!nice_value",
  "super_cookie" => "super_value",
);

$headers = headers_list();
if (($i = count($expected)) > count($headers))
{
  echo "Fewer headers are being sent than expected - aborting";
  return;
}

$i = 0;

do {
  if (strncmp(current($headers), 'Set-Cookie:', 11) !== 0)
  {
    continue;
  }
  foreach ($expected as $key => $value) {
    if (strpos(current($headers), $key) !== false) {                         // If the header contains the cookie
      if (substr($value, 0, 1) === "!") {                                   // ! is because we don't want to see the cookie value in plaintext, it must be encrypted
        if (strpos(current($headers), substr($value,1,-1)) === false) {      // If the header doesn't contain de cookie value, it's good
          $i++;
          break;
        }
        echo "Received : " . current($headers) . " and the cookie isn't encrypted . \n";
      } else {
        if (strpos(current($headers), $value) !== false) {
          $i++;
          break;
        }
        echo "Received : " . current($headers) . " and the cookie value of " . $key . " doesn't match it's value. \n";
      }
      break;
    }
  }
}
while (next($headers));

echo ($i === 4)
  ? 'OK'
  : 'A total of '. (count($expected) - $i) .' errors found.';
?>
--EXPECT--
OK
