$(document).ready(onDocLoad);

var timer = null;

function onDocLoad() {
	$("#searchButton").click(searchButtonClick);
	$("#searchBox").keyup(searchBoxKeyUpFunction);
	$("#searchBox").keypress(function(e) {
		if (e.which == 13)
			searchButtonClick();
	});
	$("#searchBox").focus(searchBoxTimerFunction);
	$("#searchBox").click(function(e){
		e.stopPropagation();
	});
	$("html").click(function() {
		$("#suggestionDiv").css("display","none");
	});

	var pos = $("#searchBox").position();
	pos.top += $("#searchBox").outerHeight();
	$("#suggestionDiv").css(pos);
}

function searchButtonClick() {
	$.getJSON("/ekatte/search.php?q=" + $("#searchBox").val(), function(data) {
		$(".resultRow").remove();
		for (var i=0;i<data.length;i++) {
			$('#resultsTable tr:last').after(buildResultTableRow(data[i]));
		}
	});
}

function buildResultTableRow(data) {
	var row = "<tr class=\"resultRow\">";
	// ekatte code
	row += "<td>"+data[0]+"</td>";
	// name + kind
	row += "<td><a href=\"viewPlace.php?ekatte="+data[0]+"\">";
	if (data[2] == 1) row += "гр. "; else
	if (data[2] == 3) row += "с. "; else
	if (data[2] == 7) row += "манастир ";
	row += data[1] + "</a></td>";
	// obstina
	row += "<td><a href=\"viewObstina.php?obst="+data[4]+"\">"+data[7]+"</a></td>";
	// kmet
	row += "<td><a href=\"viewKmetstvo.php?kmet="+data[5]+"\">"+((data[8]!=null)?data[8]:data[5])+"</a></td>";
	// oblast
	row += "<td><a href=\"viewOblast.php?obl="+data[3]+"\">"+data[6]+"</a></td>";
	// region
	row += "<td><a href=\"viewRegion.php?reg="+data[9]+"\">"+data[10]+"</a></td>";
	row += "</tr>";
	return row;
}

function searchBoxKeyUpFunction() {
	if (timer != null) clearTimeout(timer);
	timer = setTimeout(searchBoxTimerFunction,500);
}

function searchBoxTimerFunction() {
	timer = null;
	if ($("#searchBox").val() == "") return;
	$.getJSON("/ekatte/suggest.php?q=" + $("#searchBox").val(), function(data) {
		var text = "";
		for (var i=0;i<data.length;i++) {
			text += buildSuggestionLink(data[i][0]);
		}
		$("#suggestionDiv").html(text);
		$(".suggestionLink").click(suggestionLinkClick);
		$("#suggestionDiv").css("display","block");
	});
}

function buildSuggestionLink(str) {
	return "<a href=\"#\" data-suggested=\""+str+"\" class=\"suggestionLink\">" + str + "</a>";
}

function suggestionLinkClick(event) {
	event.stopPropagation();
	$("#searchBox").val($(this).data("suggested"));
	$("#suggestionDiv").css("display","none");
	searchButtonClick();
}