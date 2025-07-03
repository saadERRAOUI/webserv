<?php
echo "Content-type: text/html\r\n\r\n";
echo "<html><head><title>PHP CGI Test</title></head><body>";
echo "<h1>PHP CGI is working!</h1>";
echo "<p>Current Date and Time: " . date("Y-m-d H:i:s") . "</p>";
phpinfo();
echo "</body></html>";
?>