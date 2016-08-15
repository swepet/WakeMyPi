	Project WakeMyPi v1.0
	---------------------
	An arduino pro mini, connected to a raspberry pi zero by the i2c lines.
	The arduino pro mini acts as a i2c slave. It has an timer that can be 
	configured and started by i2c. When the timer runs out, the i2c clock 
	line is pulled low for half a secound, that will wakeup a shut down pi. 
	("sudo shutdown -h now")

	The arduino have i2c address 0x04 and it have 4 register, that can be
	red or written to.
	
	The registers are
	1: hours
	2: minutes
	3: secound
	4: enable  <-- when enable is set to 1, the timer starts counting down

	The purpuse of this project is to easy be able to shut down a remote 
	battery powerd pi zero for a certain amount of time to conserve power.
	It will still consume a small amout of current, since the pi is in 
	halted state and the arduino takes some to. But i requires very little
	extra hardware, just an arduino pro mini and 4 wires.

	And in the future the arduino can also be used to read the battery 
	voltage, light sensor, motion sensor etc and make it avalible to the pi 
	by i2c, that something a RTC with alarm can't do.

	I use the pi zero v1.3 (the one with camera connector), but i think it
	also will work with other newer pi model that have the firmware that 
	can be woken from halt by connecting GPIO3 (SCL i2c) to GND when halted.

	I use RASPBIAN JESSIE LITE on my pi. I wrote a small python program to
	set the registers on the arduino with. Make sure i2c is enabled
	on your pi and that you have python-smbus installed. The python program
	is i2c.py. It takes one or two arguments. 

	To read a register do
	python i2c.py [register]

	To write to a register do
	python i2c.py [register] [data]

	Example, to set timer to 1 hour 5 minits and 35 sec and start it do
	
	python i2c.py 1 1  #this sets register 1 that is hours to 1
	python i2c.py 2 5  #this sets mins to 5
	python i2c.py 3 35 #this sets sec to 35

	to check how many min that was set
	python i2c.py 2    #in this case this will return 5

	When timer is set, lets start the countdown
	python i2c.py 4 1   #this sets the enable register to 1 and timer starts.

	Now the pi can be shut down.
	sudo shutdown -h now

	After 1 hour 5 min and 35 sec it will wake up again.

	
	2016-08-15
	by Peter Lehnér (petleh82 @ gmail.com)

