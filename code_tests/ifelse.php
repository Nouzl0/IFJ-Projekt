<?php
    declare(strict_types=1);

//    // test_0 - simple if - result (0)
//    $test_0 = 0;
//    if ($test_0 == 0) {
//        write($test_0);
//    }
//
//    // test_1 - simple if skip - result (1)
//    $test_1 = 1;
//    if ($test_1 == 0) {
//        write($test_1);
//    }
//    write($test_1);
//
//    // test_2 - simple if else (do else block) - result (0)
//    $test_2 = 0;
//    if ($test_2 == 1) {
//        write($test_2);
//    } else {
//        write($test_2);
//    }
//
//    // test_3 - simple if else skip (do if block) - result (1)
//    $test_3 = 1;
//    if ($test_3 == 1) {
//        write($test_3);
//    } else {
//        write($test_3);
//    }
//
//    // test_4 - if inside if's - result (5)
//    $test_4 = 1;
//    if ($test_4 == 1) {
//        $test_4 = 2;
//
//        if ($test_4 == 2) {
//            $test_4 = 3;
//            
//            if ($test_4 == 3) {
//                $test_4 = 4;
//            }
//        }
//        $test_4 = $test_4 + 1;
//    }
//    write($test_4);
//
//    // test_5 - ifelse inside ifelse's - result (6)
//    $test_5 = 1;
//    if ($test_5 == 1) {
//        $test_5 = 2;
//
//        if ($test_5 == 2) {
//            $test_5 = 4;
//            
//            if ($test_5 == 3) {
//                $test_5 = 4;
//            } else {
//                $test_5 = 5;
//            }
//        } else {
//            $test_5 = 6;
//        }
//        $test_5 = $test_5 + 1;
//    } else {
//        $test_5 = 7;
//    }
//    write($test_5);
//
//    // test_6 - if else with no simple values - result (1)
//    if (1 == 1) {
//        $test_6 = 1;
//        write($test_6);
//    } else {
//        $test_6 = 2;
//        write($test_6);
//    }
//
//    // test_7 - if else with no different simple values - result (11)
//    if ("a" == "a") {
//        $test_7 = 1;
//        write($test_7);
//    } else {
//        $test_7 = 0;
//        write($test_7);
//    }
//    if (2.5 == 2.5) {
//        $test_7 = 1;
//        write($test_7);
//    } else {
//        $test_7 = 0;
//        write($test_7);
//    }
//
//    // test_8 - with equestion - result (1)
//    $test_8 = 1;
//    if (($test_8 * 3) == ($test_8 * 2) + 1)  {
//        $test_8 = 1;
//        write($test_8);
//    } else {
//        $test_8 = 0;
//        write($test_8);
//    }
//
//    // test_9 - testing different logic operands - result (11111111)
//    $test_9 = 1; $test_9_1 = 0; $test_9_2 = 0;
//    
//    if ($test_9_1 == $test_9_2) {
//        write($test_9);
//    }
//  
//    if ($test_9 !== $test_9_1) {
//        write($test_9);
//    }
//    if ($test_9 > $test_9_1) {
//        write($test_9);
//    }
//    if ($test_9_1 < $test_9) {
//        write($test_9);
//    }
//    if ($test_9 >= $test_9_1) {
//        write($test_9);
//    }
//    if ($test_9_2 >= $test_9_1) {
//        write($test_9);
//    }
//    if ($test_9_1 <= $test_9) {
//        write($test_9);
//    }
//    if ($test_9_2 <= $test_9_1) {
//        write($test_9);
//    }
//
//    // test_10 - testing different logic operands - result (11111111)
//    $test_10 = 1; $test_10_1 = 1.0; $test_10_2 = 0.0;
//
//    if (($test_10_1 / 2.0) == $test_10_2) {
//        write($test_10);
//    }
//    if ($test_10 + 1 > $test_10 + 1) {
//        write($test_10);
//    }
//    if (($test_10 * 2) < ($test_10 * 2)) {
//        write($test_10);
//    }
//    if (($test_10 / 5) >= ($test_10_1 / 5)) {
//        write($test_10);
//    }
//    if (($test_10_2 + 1.25) >= $test_10_1 + 45.5) {
//        write($test_10);
//    }
//    if ($test_10_1 <= ($test_10_1 * 2.1)) {
//        write($test_10);
//    }
//    if (($test_10_2 + 46.65) <= $test_10_1) {
//        write($test_10);
//    }
//
//    // test_11 - testing something complicated - result (67)
//    $test_11 = 1;
//
//    if ($test_11 == 1) {
//        $test_11 = 2;
//        if ($test_11 !== (2 / 2)) {
//            $test_11 = 2.5;
//            if ($test_11 == 3.5) {
//
//                $test_11 = 4;
//            } else {
//                $test_11 = 5;
//                if (($test_11 / 2) == 2.5) {
//                    $test_11 = 6;
//                    write($test_11);
//                } else {
//                    $test_11 = 1;
//                    write($test_11);
//                }
//            }
//        }
//        $test_11 = $test_11 + 1;
//    }
//    write($test_11);


    // test_12 - string compare - result (text)
    $test_12 = "text";
    $test_12_1 = "text";

    if ($test_12 == $test_12_1) {
        write($test_12);
    }   

    // test_13 - testing nil - result (1)
    $a = readi();
    if ($a !== null) {
        write("0");
    } else {
        write("1");
    }

?>
