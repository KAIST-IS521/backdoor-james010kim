void main(void) {

	char* buf = malloc( 0xf0 );  // 0xf0
	printf( "User:␣" );

	gets( buf );

	if ( strcmp( buf, "user" ) == 0 ) {
		printf( "Password:␣" );
		gets( buf );
		if ( strcmp( buf, "password" ) == 0 ) {
				puts( "Success" ); 
			} else {
				puts( "Failure" ); 
			}
	} else {
		puts( "Failure" );
	}

	exit( 0 ); 
}
