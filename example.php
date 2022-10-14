<?php
	/*
	blokovy komentar
	*/
	//radkovy komentar
	declare(strict_types=1);
	function concat(string $x, string $y): string {
		$x = $x . $y;
		return $x . " " . $y;
	}
	$a = "ahoj ";
	$ret = concat($a, "svete");
	$b = 45 + 83 + 120 / 20;
	write($ret, $a);
	returnx()
?>