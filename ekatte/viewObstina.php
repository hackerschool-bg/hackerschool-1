<?php
header('Content-Type: text/html; charset=utf-8');

include "dbConnectionSettings.php";

$conn = pg_connect($pgConnectionString);
if (!$conn) {
	die(json_encode(array("error"=>"Could not connect to database")));
}

if (!isset($_GET['obst']) || strlen($_GET['obst']) != 5) {
	die ("No such obstina!");
}

$queryResult = pg_query_params($conn,'select ek_obst.*, ek_atte.name as center_name, ek_atte.kind as center_kind from ek_obst left join ek_atte on ek_obst.ekatte_center=ek_atte.ekatte_code where ek_obst.obstina_code = $1',array($_GET['obst']));

$obst = 0;
if (pg_num_rows($queryResult) != 1) {
	die("No such obstina!");
}
$obst = pg_fetch_row($queryResult);
if ($obst[4] == 1) $obst[4] = "гр. "; else
if ($obst[4] == 3) $obst[4] = "с. "; else
if ($obst[4] == 7) $obst[4] = "манастир ";

$queryResult = pg_query_params($conn, 'select * from ek_atte where obstina = $1',array($_GET['obst']));
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
<h4>Община</h4>
<table>
	<tr><td>Код</td><td><?php echo $_GET['obst']; ?></td></tr>
	<tr><td>Име</td><td><?php echo $obst[2]; ?></td></tr>
	<tr><td>Център</td><td><a href="viewPlace.php?ekatte=<?php echo $obst[1]; ?>"><?php echo $obst[4].$obst[3];?></a></td></tr>
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