<?php

echo "Snuffleupagus Test Script\n";

// Check if Snuffleupagus is loaded
if (!extension_loaded('snuffleupagus')) {
    die("Snuffleupagus extension is not loaded.\n");
}
echo "Snuffleupagus extension is loaded.\n";

// Test basic configuration loading (even with an empty rules file)
echo "Testing configuration loading...\n";
$config_file = ini_get("snuffleupagus.config_file");
if ($config_file) {
    echo "Configuration file path: " . $config_file . "\n";
    if (file_exists($config_file)) {
        echo "Configuration file exists.\n";
    } else {
        echo "WARNING: Configuration file does not exist at path: " . $config_file . "\n";
    }
} else {
    echo "WARNING: snuffleupagus.config_file INI setting not found.\n";
}

// Simulate some function calls that Snuffleupagus might monitor
echo "Simulating function calls...\n";
$a = "hello";
$b = "world";
$c = $a . $b; // String concatenation
echo "Concatenated string: " . $c . "\n";

// Simulate a file inclusion
// Create a dummy file to include
file_put_contents('/tmp/dummy_include.php', '<?php echo "Dummy include executed.\n"; ?>');
echo "Including /tmp/dummy_include.php...\n";
include '/tmp/dummy_include.php';
unlink('/tmp/dummy_include.php');

// Simulate some superglobal interactions (even if no rules are targeting them yet)
echo "Simulating superglobal access...\n";
$_GET['param1'] = 'value1';
$_POST['data'] = 'test_data';
$_COOKIE['session_id'] = 'random_session_id';

echo "Reading superglobals:\n";
echo "_GET['param1']: " . ($_GET['param1'] ?? 'not set') . "\n";
echo "_POST['data']: " . ($_POST['data'] ?? 'not set') . "\n";
echo "_COOKIE['session_id']: " . ($_COOKIE['session_id'] ?? 'not set') . "\n";

// Intentionally call a non-existent function to see if it's handled
// Snuffleupagus might have hooks around function execution/errors
echo "Attempting to call a non-existent function (expecting a PHP error)...\n";
if (function_exists('a_function_that_does_not_exist')) {
    // This block should not be reached
    a_function_that_does_not_exist();
} else {
    echo "Function 'a_function_that_does_not_exist' does not exist, as expected.\n";
}

// Loop a few times to increase chances of leaks showing up with repeated operations
echo "Looping for a few iterations...\n";
for ($i = 0; $i < 10; $i++) {
    $temp_string = "iteration_string_" . $i;
    $temp_string .= md5($temp_string); // Some string operations
    // Potentially add more operations here that Snuffleupagus might inspect
}
echo "Loop finished.\n";

echo "Snuffleupagus test script finished.\n";

?>
