<?php
    declare(strict_types=1);

    function fibonacci(int $number): int {
        
        if ($number == 0) {
            return 0;
        } else {
            if ($number == 1) {
                return 1;
            } else {
                return fibonacci($number - 1) + fibonacci($number - 2);
            }
        } 
    }

    $number = 10;
    $counter = 0;

    while ($counter < $number) {

        // counter
        $counter = $counter + 1;

        // fibonacci
        write(fibonacci($number), " ");
    }

?>