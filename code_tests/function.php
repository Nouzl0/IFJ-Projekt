<?php
    declare(strict_types=1);


    // test 0 - simple function - result (1234)
    function test_0_write(int $x, int $y): void {
        $x = $x + 1;
        $x = $x + 2;
        
        write($x);
        write($y);    
    }

    // test 1 - simple recursion with return - result (12345)
    function test_1_write(int $x): void {
        $x = $x + 1;
        write($x);

        if ($x !== 5) {
            test_1_write($x);
        } else {
            return;
        }
    }
    $x = 0;
    test_1_write($x);

    // test 2 - simple function with return value - result (2)
   function test_2_add(int $x, int $y): int {
         $x = $x + $y;
         return $x;
    }
    $test_2 = test_2_add(1, 1);
    write($test_2);

    // test 3 - simple function with arithmetics and return result (2)
    function test_3_add(int $x, int $y, int $z): int {
        return $x + $y;
    }
    $test_3 = test_3_add(1, 1, 1);
    write($test_3);

    // test 4 - simple function with recursion result (2)
    function test_4_add(int $x): int {
        
        $x = $x + 1;

        if ($x == 1) {
            $x = test_4_add($x);
            write($x);
        }

        return $x;
    }
    $test_4 = test_4_add(0);

?>