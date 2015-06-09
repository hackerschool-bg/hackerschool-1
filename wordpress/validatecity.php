<?php
// searches for a valid city based on a query parameter $_GET['city'] and optional $_GET['region']
include 'eventsearchsettings.php';
date_default_timezone_set('America/New_York');

if (!isset($_GET['city']) || $_GET['city'] == "") {
	echo "[]";
	exit();
}

$dbconn = mysql_connect($dbhost,$dbuser,$dbpass);

if (!$dbconn) {
	die('Failed to connect to database! ' . mysql_error());
}

if (!mysql_select_db($dbname,$dbconn)) {
	$msg = 'Cannot use ' . $dbname . ': ' . mysql_error();
	mysql_close($dbconn);
	die($msg);
}
$exploded = explode(' ', $_GET['city']);
for($i=0;$i<count($exploded);$i++) {
	$exploded[$i] = '+' . mysql_real_escape_string($exploded[$i]) . '*';
}
$query = 'select distinct(city),region from events where match(city) against("'.implode(' ',$exploded).'" in boolean mode)';
if (isset($_GET['region'])) {
	$query .= ' and region = \'' . mysql_real_escape_string($_GET['region']) . '\'';
}
$results = mysql_query($query,$dbconn);
if (!$results) {
	$msg = 'Failed to get cities from database: ' . mysql_error();
	mysql_close($dbconn);
	die($msg);
}

$arrayForJson = array();
while ($row = mysql_fetch_assoc($results)) {
	$tArray = array();
	$tArray["city"] = json_encode($row['city']);
	$tArray["region"] = json_encode($row['region']);
	$tArray["city"] = substr($tArray["city"],1,strlen($tArray["city"])-2);
	$tArray["region"] = substr($tArray["region"],1,strlen($tArray["region"])-2);
	array_push($arrayForJson,$tArray);
}

echo json_encode($arrayForJson);

mysql_close($dbconn);

?>
