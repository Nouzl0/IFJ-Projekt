<?php
    declare(strict_types=1);

//    // test_0 - simple while - result (5)
//    $test_0 = 0;
//    while ($test_0 < 5) {
//        $test_0 = $test_0 + 1;
//    }
//    write($test_0);

    // test_1 - simple while with fractions - result (1)
//    $test_1 = 0.00; $test_1_1 = 1;
//    while ($test_1 < 1.00) {
//        $test_1 = $test_1 + 0.5;
//        write($test_1_1);
//    }
//    write($test_1);

    // test_2 - while, with definition inside - result (12)
    $test_2 = 0;
    while ($test_2 < 10) {
        $test_2 = $test_2 + 1;
        $test_2_1 = 0;
        while ($test_2_1 < 2) {
            $test_2_1 = $test_2_1 + 1;
            $test_2 = $test_2 + 1;
        }
    }
    write($test_2);

//    // test_3 - while with different definitions inside, with ifelse - result (20)
//    $test_3 = 0;
//    while ($test_3 < 10) {
//        $test_3 = $test_3 + 1;
//        $test_3_1 = 0;
//        while ($test_3_1 !== 2) {
//            if ($test_3_1 == 0) {
//                $test_3_1 = 2;
//                $test_3_2 = 1;
//                $test_3_3 = 3;
//                $test_3 = $test_3 + 1;
//            } else {
//                $test_3_4 = 5;
//            }
//        }
//    }
//    write($test_3);

    


?>