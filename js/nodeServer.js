var socket = require( 'socket.io' );
var express = require( 'express' );
var http = require( 'http' );

var app = express();
var server = http.createServer( app );

var io = socket.listen( server );

var pi_app = express();
var pi_server = http.createServer( app );

var pi_io = socket.listen( pi_server );

var push_to_pi = []; // Array of commands to push to pi

pi_io.sockets.on( 'connection', function( client ) {
	client.on( 'poll', function( data ) {
		if( push_to_pi.length != 0 ) {
			pi_io.sockets.emit( 'command', push_to_pi.shift() );
		}
	});
});

pi_server.listen( 8081 );

io.sockets.on( 'connection', function( client ) {	
	client.on( 'command', function( data ) {
		if ( data.type == 'preset' ){
			push_to_pi.push(data);
		}else{
			push_to_pi.push({
				type: 'custom',
				data: data.index + ' ' + data.r + ' ' + data.g + ' ' + data.b + '\n'
			});
		}
		// io.sockets.emit( 'message', { name: data.name, message: data.message } );
	});
});

server.listen( 8080 );