var socket = io.connect( 'http://pi.hacksocnotts.co.uk:8080' );

$( '#presets>button' ).bind( 'click', function(){
	var preset = $( this ).data( 'preset' );
	console.log(preset);
	var jsonObject = {
		type: 'preset',
		method: preset,
		loop: $( this ).data( 'loop' )
	};
	socket.emit( 'command', jsonObject);
});
