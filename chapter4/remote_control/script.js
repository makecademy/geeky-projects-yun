// Stop the robot
function stopRobot(){

	$.get( "update_state.php", { 
		command: "stop"} );
}

// Full speed forward
function fullForward(){

	$.get( "update_state.php", { 
		command: "fullfw"} );
}

// Turn left for a while, and stop
function turnLeft(){

	$.get( "update_state.php", { command: "turnleft"} );

	sleep(350);

	$.get( "update_state.php", { command: "stop"} );


}

// Turn right for a while, and stop
function turnRight(){

	$.get( "update_state.php", { command: "turnright"} );

	sleep(350);

	$.get( "update_state.php", { command: "stop"} );
}

// Sleep function
function sleep(milliseconds) {
  var start = new Date().getTime();
  for (var i = 0; i < 1e7; i++) {
    if ((new Date().getTime() - start) > milliseconds){
      break;
    }
  }
}

