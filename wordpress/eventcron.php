<?php
// fetches events from the Eventful API
// gets all pages sequentially, see the Java program for a threaded solution
include 'eventsearchsettings.php';
include 'eventcronfunctions.php';

date_default_timezone_set('America/New_York');

$dbconn = mysql_connect($dbhost,$dbuser,$dbpass);

if (!$dbconn) {
	die('Failed to connect to database! ' . mysql_error());
}

if (!mysql_select_db($dbname,$dbconn)) {
	$msg = 'Cannot use ' . $dbname . ': ' . mysql_error();
	mysql_close($dbconn);
	die($msg);
}

if (!deletePastEvents($dbconn)) {
	$msg = 'Failed to delete past events from database! ' . mysql_error();
	mysql_close($dbconn);
	die($msg);
}

$eventfulBaseURL = 'http://api.eventful.com/rest/events/search?';
$currentPage = 1;
$totalPages = 1;

while ($currentPage <= $totalPages) {
	$searchSettings['page_number'] = $currentPage;
	$fetchURL = $eventfulBaseURL . http_build_query($searchSettings,'','&',PHP_QUERY_RFC3986); //PHP_QUERY_RFC3986 = %20 for spaces
	$contents = file_get_contents($fetchURL);
	$rootXML = NULL;
	try {
		$rootXML = new SimpleXMLElement($contents);
	} catch (Exception $e) {
		mysql_close($dbconn);
		die('Eventful returned malformed XML');
	}
	$totalPages = getPageCount($rootXML);
	$events = parsePageOfEvents($rootXML);
	if (!insertEventsInDB($dbconn,$events)) {
		$msg = 'Error while inserting events in db: ' . mysql_error();
		mysql_close($dbconn);
		die($msg);
	}
	$currentPage++;
}

mysql_close($dbconn);

?>
