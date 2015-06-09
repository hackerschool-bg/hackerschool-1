<?php
header('Content-Type: text/html; charset=utf-8');

include "dbConnectionSettings.php";

$conn = pg_connect($pgConnectionString);
if (!$conn) {
	die(json_encode(array("error"=>"Could not connect to database")));
}

if (!isset($_GET['obl']) || strlen($_GET['obl']) != 3) {
	die ("No such oblast!");
}

$queryResult = pg_query_params($conn,'select ek_obl.*, ek_atte.name as center_name, ek_atte.kind as center_kind, ek_reg2.name as region_name from ek_obl left join ek_atte on ek_obl.ekatte_center=ek_atte.ekatte_code left join ek_reg2 on ek_obl.region2_code = ek_reg2.region_code where ek_obl.oblast_code = $1',array($_GET['obl']));

$obl = 0;
if (pg_num_rows($queryResult) != 1) {
	die("No such oblast!");
}
$obl = pg_fetch_row($queryResult);
if ($obl[5] == 1) $obl[5] = "гр. "; else
if ($obl[5] == 3) $obl[5] = "с. "; else
if ($obl[5] == 7) $obl[5] = "манастир ";

$queryResult = pg_query_params($conn, 'select * from ek_atte where oblast = $1',array($_GET['obl']));
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
<h4>Област</h4>
<table>
	<tr><td>Код</td><td><?php echo $_GET['obl']; ?></td></tr>
	<tr><td>Име</td><td><?php echo $obl[2]; ?></td></tr>
	<tr><td>Център</td><td><a href="viewPlace.php?ekatte=<?php echo $obl[1]; ?>"><?php echo $obl[5].$obl[4];?></a></td></tr>
	<tr><td>Регион</td><td><a href="viewRegion.php?reg=<?php echo $obl[3];?>"><?php echo $obl[3].' - '.$obl[6];?></a></td></tr>
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