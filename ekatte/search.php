<?php

include "dbConnectionSettings.php";

function searchByPlaceName($conn,$searchString) {
	$queryResult = pg_query_params($conn,'select ek_atte.*,ek_obl.name as obl_name,ek_obst.name as obst_name,ek_kmet.name as kmet_name,ek_reg2.region_code,ek_reg2.name as region_name from ek_atte left join ek_obl on ek_atte.oblast=ek_obl.oblast_code left join ek_obst on ek_atte.obstina=ek_obst.obstina_code left join ek_kmet on ek_atte.kmetstvo=ek_kmet.kmetstvo_code left join ek_reg2 on ek_obl.region2_code=ek_reg2.region_code where ek_atte.name ilike $1 order by ek_atte.name',array('%'.$searchString.'%'));
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