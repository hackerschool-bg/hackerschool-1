var validatedState = "";
var validatedCity = "";

var stateRequest=0;
var cityRequest=0;

var stateTimer = null;
var cityTimer = null;

function stateTimerFunction() {
	stateTimer = null;
	$.getJSON("/wp/validatestate.php?region=" + $("#stateInput").val(), function(response) {
		data = response.results;
		if (data.length == 0) {
			//$("#stateSuggestion").html("No such state!");
			$("#stateSuggestion").css("display","none");
			return;
		}
		if (data.length == 1) {
			$("#stateInput").val(data[0]);
			validatedState = data[0];
			$("#stateSuggestion").css("display","none");
			return;
		}
		var suggestion="";
		for (i=0;i<data.length;i++) {
			suggestion += createStateSuggestionLink(data[i]) + " ";
		}
		$("#stateSuggestion").html(suggestion);
		$("#stateSuggestion a").css("width",$("#stateInput").outerWidth()-22);
		$("#stateSuggestion").css("display","block");
		$(".stateSuggestion").click(stateSuggestionClick);
	});
}

function stateInputKeyup() {
	if (stateTimer != null) clearTimeout(stateTimer);
	stateTimer = setTimeout(stateTimerFunction,500);
}

function cityTimerFunction() {
	cityTimer = null;
	var url = "/wp/validatecity.php?city="+$("#cityInput").val();
	if (validatedState != "") {
		url = url + "&region=" + validatedState;
	}
	$.getJSON(url, function(data) {
		if (data.length == 0) {
			$("#citySuggestion").css("display","none");
			return;
		}
		if (data.length == 1) {
			$("#cityInput").val(data[0].city);
			$("#stateInput").val(data[0].region);
			validatedCity = data[0].city;
			validatedState = data[0].region;
			return;
		}
		var suggestion="";
		for (i=0;i<data.length;i++) {
			suggestion += createCitySuggestionLink(data[i]) + " ";
		}
		$("#citySuggestion").html(suggestion);
		$("#citySuggestion a").css("width",$("#cityInput").outerWidth()-22);
		$("#citySuggestion").css("display","block");
		$(".citySuggestion").click(citySuggestionClick);
	});
}

function cityInputKeyup() {
	if (cityTimer != null) clearTimeout(cityTimer);
	stateTimer = setTimeout(cityTimerFunction,500);
}

function stateSuggestionClick() {
	validatedState = $(this).data('state');
	$("#stateInput").val(validatedState);
	$("#stateSuggestion").html("");
	$("#stateSuggestion").css("display","none");
}

function citySuggestionClick() {
	validatedState = $(this).data('state');
	$("#stateInput").val(validatedState);
	validatedCity = $(this).data('city');
	$("#cityInput").val(validatedCity);
	$("#stateSuggestion").html("");
	$("#stateSuggestion").css("display","none");
	$("#citySuggestion").html("");
	$("#citySuggestion").css("display","none");
}

function loadmap() {

	$("#stateSuggestion").css("top",$("#stateInput").height());
	var pos = $("#stateInput").position();
	pos.top += $("#stateInput").outerHeight();
	$("#stateSuggestion").css(pos);
	$("#stateSuggestion").css("width",$("#stateInput").outerWidth());

	$("#citySuggestion").css("top",$("#cityInput").height());
	var pos = $("#cityInput").position();
	pos.top += $("#cityInput").outerHeight();
	$("#citySuggestion").css(pos);
	$("#citySuggestion").css("width",$("#cityInput").outerWidth());
	
	$("#stateInput").keyup(stateInputKeyup);
	$("html").click(function(){
		$("#stateSuggestion").css("display","none");
	});
	$("#stateSuggestion").click(function(event){
		event.stopPropagation();
	});
	$("#stateInput").focus(function(event){
		event.stopPropagation();
		if ($(this).val() != "") {
			stateInputKeyup();
		}
	});

	$("#cityInput").keyup(cityInputKeyup);
	$("html").click(function() {
		$("#citySuggestion").css("dislay","none");
	});
	$("#citySuggestion").click(function(event){
		event.stopPropagation();
	});
	$("#cityInput").focus(function(event) {
		event.stopPropagation();
		if ($(this).val() != "") {
			cityInputKeyup();
		}
	});

	var mapOptions = {
		center: { lat: 39.8282, lng: -98.5795},
		zoom: 3
	};
	
	var map = new google.maps.Map(document.getElementById('map-canvas'),mapOptions);
	var markers = [];
	var infowindows = [];
	
	$("#showButton").click(function() {
		// clear existing markers and info 
		while (infowindows.length) { infowindows.pop().setMap(null); }
		while (markers.length) { markers.pop().setMap(null); }
		$("#errorDiv").html("");
		if (validatedState == "") {
			$("#errorDiv").html("You must at least select a valid state");
		}
		var url = "/wp/getevents.php?region=" + validatedState;
		if (validatedCity != "") {
			url += "&city=" + validatedCity;
		}
		$.getJSON(url, function( data ) {
			for (i = 0;i<data.length;i++) {
				var myLatlng = new google.maps.LatLng(data[i].latitude,data[i].longitude);
				var contentString = '<div class="content">'+
				  '<h4 style="margin: 5px;">'+data[i].name+'</h4>'+
				  '<div>'+
				  '<p>'+data[i].description+'</p>'+
				  '<p><a href="'+data[i].url+'">'+
				  'Full details</a></p>'+
				  '</div>'+
				  '</div>';
				infowindows[i] = new google.maps.InfoWindow({
						content: contentString
				});

				markers[i] = new google.maps.Marker({
						position: myLatlng,
						map: map,
						title: ((typeof data[i].name)=="string")?data[i].name:"No title"
				});
				google.maps.event.addListener(markers[i], 'click', function(key) {
					return function() { infowindows[key].open(map,markers[key]); }
				}(i));
			}
		});
	});

}

function createStateSuggestionLink(state) {
	return "<a href=\"#\" class=\"stateSuggestion\" data-state=\""+state+"\">" + state + "</a>";
}

function createCitySuggestionLink(obj) {
	return "<a href=\"#\" class=\"citySuggestion\" data-city=\""+obj.city+"\" data-state=\""+obj.region+"\">" + obj.city + ", " + obj.region + "</a>";
}
