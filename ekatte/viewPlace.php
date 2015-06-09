<?php
header('Content-Type: text/html; charset=utf-8');

include "dbConnectionSettings.php";

$conn = pg_connect($pgConnectionString);
if (!$conn) {
	die(json_encode(array("error"=>"Could not connect to database")));
}

if (!isset($_GET['ekatte']) || strlen($_GET['ekatte']) != 5) {
	die ("No such place!");
}

$queryResult = pg_query_params($conn,'select ek_atte.*,ek_obl.name as obl_name,ek_obst.name as obst_name,ek_kmet.name as kmet_name,ek_reg2.region_code,ek_reg2.name as region_name from ek_atte left join ek_obl on ek_atte.oblast=ek_obl.oblast_code left join ek_obst on ek_atte.obstina=ek_obst.obstina_code left join ek_kmet on ek_atte.kmetstvo=ek_kmet.kmetstvo_code left join ek_reg2 on ek_obl.region2_code=ek_reg2.region_code where ek_atte.ekatte_code = $1',array($_GET['ekatte']));
if (pg_num_rows($queryResult) != 1) {
	die ("No such place!");
}
$row = pg_fetch_row($queryResult);
pg_close($conn);

//ekatte_code	name	kind	oblast	obstina	kmetstvo	obl_name	obst_name	kmet_name	region_code	region_name
$ekatte_code = $row[0];
$place_name = $row[1];
$kind = $row[2];
$oblast_code = $row[3];
$obstina_code = $row[4];
$kmetstvo_code = $row[5];
$oblast_name = $row[6];
$obstina_name = $row[7];
$kmetstvo_name = $row[8];
$region_code = $row[9];
$region_name = $row[10];

if ($kind == 1) $kind = "гр. "; else
if ($kind == 3) $kind = "с. "; else
if ($kind == 7) $kind = "манастир ";
$oblast = $oblast_code . " - " . $oblast_name;
$obstina = $obstina_code . " - " . $obstina_name;
$region = $region_code . " - " . $region_name;
$kmetstvo = $kmetstvo_code;
if ($kmetstvo_name != NULL) $kmetstvo .= " - " . $kmetstvo_name;
?>
<!DOCTYPE html>
<html>
<head>
	<link rel="stylesheet" type="text/css" href="styles.css">
	<meta http-equiv="content-type" content="text/html; charset=utf-8" />
</head>
<body>
	<h4>Населено място</h4>
<table>
	<tr><td>Име</td><td><?php echo $kind . $place_name; ?></td></tr>
	<tr><td>ЕКАТЕ</td><td><?php echo $ekatte_code; ?></td></tr>
	<tr><td>Кметство</td><td><a href="viewKmetstvo.php?kmet=<?php echo $kmetstvo_code; ?>"><?php echo $kmetstvo; ?></a></td></tr>
	<tr><td>Община</td><td><a href="viewObstina.php?obst=<?php echo $obstina_code; ?>"><?php echo $obstina; ?></td></tr>
	<tr><td>Област</td><td><a href="viewOblast.php?obl=<?php echo $oblast_code; ?>"><?php echo $oblast; ?></td></tr>
	<tr><td>Регион</td><td><a href="viewRegion.php?reg=<?php echo $region_code; ?>"><?php echo $region; ?></td></tr>
</table>
</body>
</html>