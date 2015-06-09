<?php
// searches for a state based on a query parameter $_GET['region']
include 'eventsearchsettings.php';
date_default_timezone_set('America/New_York');

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

$exploded = explode(' ', $_GET['region']);
$forImplosion = array();
for($i=0;$i<count($exploded);$i++) {
	if (trim($exploded[$i]!="")) {
		array_push($forImplosion,'+' . mysql_real_escape_string($exploded[$i]) . '*');
	}
}
$dbquery = 'select distinct(region) from events where match(region) against(\''.implode(' ',$forImplosion).'\' in boolean mode) and region like \'%'.mysql_real_escape_string(trim($_GET['region'])).'%\'';
$results = mysql_query($dbquery,$dbconn);
//region like \'%' . mysql_real_escape_string($_GET['region']) . '%\'',$dbconn);
if (!$results) {
	$msg = 'Failed to get regions from database: ' . mysql_error();
	mysql_close($dbconn);
	die($msg);
}

$arrayForJson = array();
while ($row = mysql_fetch_assoc($results)) {
	$t = json_encode($row['region']);
	array_push($arrayForJson,substr($t,1,strlen($t)-2));
}

echo json_encode(array("query"=>$_GET['region'],"results"=>$arrayForJson,"dbquery"=>$dbquery));

mysql_close($dbconn);

?>
