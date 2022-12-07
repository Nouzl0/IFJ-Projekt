<?php
    declare(strict_types=1);

    // add
    function add(int $x, int $y): int {
        return $x + $y;
    }

    $num = add(add(1 + 1, 1 + 1), add(1 + 1, 1 + 1));
    write($num);

    $num = add(add(1 + 1, 1 + 1), add(1 + 1, 1 + 1));
    write($num);

?>  