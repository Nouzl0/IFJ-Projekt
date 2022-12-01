<?php
//komentar
/*
dalsi komentar
*/
declare(strict_types=1);

//fun1(fun2(),fun3(func4(),func5())); 

function getMax(int $x, int $y): int {
	if($x === $y){
		return $x;
	} else {
		return $y;
	}
}
$a = 4 * (5 + 2);

$b = (4.5 - 3.20) * (4.1 + 2.5);

$a = $a + getMax($a,20);

while($a > 5){
	$b = 5 + 3 / 4;
}


?>
