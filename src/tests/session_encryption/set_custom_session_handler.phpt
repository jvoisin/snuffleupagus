--TEST--
Set a custom session handler
--SKIPIF--
<?php if (!extension_loaded("snuffleupagus")) print "skip"; ?>
<?php if (!extension_loaded("session")) print "skip"; ?>
<?php if (PHP_VERSION_ID >= 80400) print "skip"; /* Calling session_set_save_handler() with more than 2 arguments is deprecated */ ?>
--EXTENSIONS--
session
--INI--
sp.configuration_file={PWD}/config/config_crypt_session.ini
session.save_path="/tmp"
--ENV--
return <<<EOF
REMOTE_ADDR=127.0.0.1
EOF;
--FILE--
<?php
class FileSessionHandler {
    private $savePath;

    function open($savePath, $sessionName) {
        $this->savePath = $savePath;
        if (!is_dir($this->savePath)) {
            mkdir($this->savePath, 0777);
        }

        return true;
    }

    function close() {
        return true;
    }

    function read($id) {
        return (string)@file_get_contents("$this->savePath/sess_$id");
    }

    function write($id, $data) {
        return file_put_contents("$this->savePath/sess_$id", $data) === false ? false : true;
    }

    function destroy($id) {
        $file = "$this->savePath/sess_$id";
        if (file_exists($file)) {
            unlink($file);
        }

        return true;
    }

    function gc($maxlifetime) {
        foreach (glob("$this->savePath/sess_*") as $file) {
            if (filemtime($file) + $maxlifetime < time() && file_exists($file)) {
                unlink($file);
            }
        }
        return true;
    }
}

$handler = new FileSessionHandler();
session_set_save_handler(
    array($handler, 'open'),
    array($handler, 'close'),
    array($handler, 'read'),
    array($handler, 'write'),
    array($handler, 'destroy'),
    array($handler, 'gc')
    );

// the following prevents unexpected effects when using objects as save handlers
register_shutdown_function('session_write_close');

session_start();
// proceed to set and retrieve values by key from $_SESSION
--EXPECTF--
