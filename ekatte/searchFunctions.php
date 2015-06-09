<?php

function searchByPlaceName($conn,$searchString) {
	$queryResult = pg_query_params($conn,'select * from ek_atte where name ilike $1',array('%'.$searchString.'%'));
	$results = array();
	while ($row = pg_fetch_row($queryResult)) {
		array_push($results,$row);
	}
	return $results;
}

function searchByEkatte($conn,$searchString) {
	$queryResult = pg_query_params($conn,'select * from ek_atte where ekatte_code like $1',array('%'.$searchString.'%'));
	$results = array();
	while ($row = pg_fetch_row($queryResult)) {
		array_push($results,$row);
	}
	return $results;
}

function mergePlaceArrays($arrays) {
	$places = array();
	$ekatteCodes = array();
	for ($arrI=0;$arrI<count($arrays);$arrI++) {
		for ($i=0;$i<count($arrays[$i]);$i++) {
			//if (!in_array($arrays[$i][0]))
		}
	}
}

?>