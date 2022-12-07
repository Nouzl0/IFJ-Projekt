<?php
	declare(strict_types=1);
    
//    // nefunguje fix
//    // string@retezec\032s\032lomitkem\032\092\032a\010novym\035radkem - zadanie
//    // string@retezec\032s\032lomitkem\032\092\032a\010novym#radkem - výstup
//    write("retezec s lomitkem \ a\nnovym#radkem");


    $var1 = "abcdefg";
    $var2 = 2;
    $var3 = 5;
    $x = substring($var1, $var2, $var3);
    write($x); //should be cde
?>
