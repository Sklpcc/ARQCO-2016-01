TODO: (* means in progress)

		PROBLEMS:
		- Row at max brightness in 3 colors banishes blue

		MOST IMPORTANT:
		- put all class in a single file
		- make a class to handle all effects instead inside Matrix Class
		-* finish all animations
		- add asynchronous effects button
		- increase the speed changing digitalWrite and similar functions within ISR

		LEAST IMPORTANT
		- add support not only for MEGA (interrupts for example)
		- enhance rgb (hsv support)
		- extend BCM to 8 bit resolution
		- test PWM instead of BCM
		- add more comments

	CHANGELOG:
		- All classes are now in main file due referencing compiling errors.
		- New games: tetris, pong, snake.
		- Matrix class improved to extract color info of a led.
		- List class created.
		- Printable class has been deleted.
		- Changelog has been separated.