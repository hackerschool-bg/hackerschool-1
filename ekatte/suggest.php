<?php

include "dbConnectionSettings.php";

if (!isset($_GET['q']) || strlen($_GET['q'])==0) {
	die("[]");
}

function searchByPlaceName($conn,$searchString) {
	$queryResult = pg_query_params($conn,'select distinct(name) from ek_atte where name ilike $1 order by name',array('%'.$searchString.'%'));
	$results = array();
	while ($row = pg_fetch_row($queryResult)) {
		array_push($results,$row);
	}
	return $results;
}

$conn = pg_connect($pgConnectionString);
if (!$conn) {
	die(json_encode(array("error"=>"Could not connect to database")));
}

echo json_encode(searchByPlaceName($conn,$_GET['q']));

pg_close($conn);

?>