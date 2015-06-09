<?php
header('Content-Type: text/html; charset=utf-8');

include "dbConnectionSettings.php";

$conn = pg_connect($pgConnectionString);
if (!$conn) {
	die(json_encode(array("error"=>"Could not connect to database")));
}

if (!isset($_GET['reg']) || strlen($_GET['reg']) != 4) {
	pg_close($conn);
	die ("No such region!");
}

$queryResult = pg_query_params($conn,'select name from ek_reg2 where region_code = $1',array($_GET['reg']));

if (pg_num_rows($queryResult) != 1) {
	pg_close($conn);
	die("No such oblast!");
}
$reg = pg_fetch_row($queryResult);

$queryResult = pg_query_params($conn, 'select * from ek_obl where region2_code = $1',array($_GET['reg']));
$oblasti = array();
while ($row = pg_fetch_row($queryResult)) {
	array_push($oblasti,$row);
}

pg_close($conn);

?><!DOCTYPE html>
<html>
<head>
	<link rel="stylesheet" type="text/css" href="styles.css">
	<meta http-equiv="content-type" content="text/html; charset=utf-8" />
</head>
<body>
<h4>Регион</h4>
<table>
	<tr><td>Код</td><td><?php echo $_GET['reg']; ?></td></tr>
	<tr><td>Име</td><td><?php echo $reg[0]; ?></td></tr>
</table>
<h4>Области</h4>
<div style="height: 150px; width: 300px; overflow-y: scroll;border: 1px solid black;">
<table>
	<?php
		for ($i = 0;$i<count($oblasti);$i++):
	?>
	<tr><td><a href="viewOblast.php?obl=<?php echo $oblasti[$i][0];?>"><?php echo $oblasti[$i][2];?></a></td></tr>
	<?php endfor; ?>
</table>
</div>
</body>
</html>