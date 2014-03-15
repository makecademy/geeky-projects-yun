function relayOn(){

	$.get( "update_state.php", { 
		command: "1"} );
}

function relayOff(){

	$.get( "update_state.php", { 
		command: "0"} );
}