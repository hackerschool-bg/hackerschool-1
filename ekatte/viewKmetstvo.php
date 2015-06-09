<?php
header('Content-Type: text/html; charset=utf-8');

include "dbConnectionSettings.php";

$conn = pg_connect($pgConnectionString);
if (!$conn) {
	die(json_encode(array("error"=>"Could not connect to database")));
}

if (!isset($_GET['kmet']) || strlen($_GET['kmet']) != 8) {
	die ("No such kmetstvo!");
}

$queryResult = pg_query_params($conn,'select ek_kmet.*, ek_atte.name as center_name, ek_atte.kind as center_kind from ek_kmet left join ek_atte on ek_kmet.ekatte_center=ek_atte.ekatte_code where ek_kmet.kmetstvo_code = $1',array($_GET['kmet']));

$skipDetailedInfo = false;
$kmet = 0;
if (pg_num_rows($queryResult) != 1) {
	if (substr($_GET['kmet'],-2) == "00") {
		$skipDetailedInfo = true;
	} else {
		die("No such kmetstvo");
	}
} else {
	$kmet = pg_fetch_row($queryResult);
	if ($kmet[4] == 1) $kmet[4] = "гр. "; else
	if ($kmet[4] == 3) $kmet[4] = "с. "; else
	if ($kmet[4] == 7) $kmet[4] = "манастир ";
}

$queryResult = pg_query_params($conn, 'select * from ek_atte where kmetstvo = $1',array($_GET['kmet']));
$places = array();
while ($place = pg_fetch_row($queryResult)) {
	if ($place[2] == 1) $place[2] = "гр. "; else
	if ($place[2] == 3) $place[2] = "с. "; else
	if ($place[2] == 7) $place[2] = "манастир ";
	array_push($places,$place);
}

pg_close($conn);

?><!DOCTYPE html>
<html>
<head>
	<link rel="stylesheet" type="text/css" href="styles.css">
	<meta http-equiv="content-type" content="text/html; charset=utf-8" />
</head>
<body>
<h4>Кметство</h4>
<table>
	<tr><td>Код</td><td><?php echo $_GET['kmet']; ?></td></tr>
	<?php if (!$skipDetailedInfo): ?>
	<tr><td>Име</td><td><?php echo $kmet[2]; ?></td></tr>
	<tr><td>Център</td><td><a href="viewPlace.php?ekatte=<?php echo $kmet[1]; ?>"><?php echo $kmet[4].$kmet[3];?></a></td></tr>
	<?php endif; ?>
</table>
<h4>Населени места</h4>
<div style="height: 150px; width: 300px; overflow-y: scroll;border: 1px solid black;">
<table>
	<?php
		for ($i = 0;$i<count($places);$i++):
	?>
	<tr><td><a href="viewPlace.php?ekatte=<?php echo $places[$i][0];?>"><?php echo $places[$i][2].$places[$i][1];?></a></td></tr>
	<?php endfor; ?>
</table>
</div>
</body>
</html>