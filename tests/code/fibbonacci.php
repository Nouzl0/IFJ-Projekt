<?php
    declare(strict_types=1);

    function fibbonacci(int $number): int {
        
        if ($number == 0) {
            return 0;
        } else {
            if ($number == 1) {
                return 1;
            } else {
                return fibbonacci($number - 1) + fibbonacci($number - 2);
            }
        } 
    }

    $number = 20;
    $counter = 0;

    while ($counter < $number) {
        // counter
        $counter = $counter + 1;
        $number = fibbonacci($number);

        // fibonacci
        write($number, " ");
    }

?>