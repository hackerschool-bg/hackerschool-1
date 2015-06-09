<?php
// returns events in a particular city from the database formatted as JSON
include 'eventsearchsettings.php';
date_default_timezone_set('America/New_York');

function escapeStrForJson($str) {
	$t = json_encode($str);
	return substr($t,1,strlen($str)-2);
}

if (!isset($_GET['region']) || $_GET['region'] == "") {
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

$query = 'select * from events where region = \'' . $_GET['region'] . '\'';
if (isset($_GET['city']) && $_GET['city'] != "") {
	$query .= ' and city = \'' . $_GET['city'] . '\'';
}

$results = mysql_query($query,$dbconn);

if (!$results) {
	$msg = 'Failed to get events from database: ' . mysql_error();
	mysql_close($dbconn);
	die($msg);
}

$arrayForJson = array();
while ($row = mysql_fetch_assoc($results)) {
	$eventArr = array();
	$eventArr['id'] = $row['id'];
	$eventArr['name'] = escapeStrForJson($row['name']);
	$eventArr['description'] = escapeStrForJson($row['description']);
	$eventArr['time'] = $row['startTime'];
	$eventArr['url'] = $row['url'];
	$eventArr['latitude'] = floatval($row['latitude']);
	$eventArr['longitude'] = floatval($row['longitude']);
	array_push($arrayForJson,$eventArr);
}

echo json_encode($arrayForJson);

mysql_close($dbconn);

?>
