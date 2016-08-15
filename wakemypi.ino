/*

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



*/







#include <Wire.h>

#define SLAVE_ADDRESS 0x04

#define debug //comment out this line to disable serial debug output

int reg = 0;
int data = 0;

int w_hour = 0;
int w_min = 0;
int w_sec = 0;
bool w_enable = 0;




void setup() {


#ifdef debug
	Serial.begin(9600); 
#endif

// initialize i2c as slave
Wire.begin(SLAVE_ADDRESS);
Wire.onReceive(receiveData);
Wire.onRequest(sendData);

}

void loop() {


	delay(1000); //this can be replaced with some power saving sleep mode if that is important for you

	
	//if the timer is enabled then the clock is counting down
	if (w_enable == 1){

		w_sec--;

		if(w_sec == -1){
			w_min--;
			w_sec=59;
		}

		if(w_min == -1){
			w_hour--;
			w_min=59;
		}


		if(w_hour == -1){ //there is no more time left, time to wake up

			#ifdef debug
			Serial.println();
				Serial.println("Waking up the pi");			
			#endif
			
			w_hour=0;
			w_min=0;
			w_sec=0;
			w_enable=0;
			
			wake_pi();
		}



	#ifdef debug
		Serial.print(w_hour);
		Serial.print(":");
		Serial.print(w_min);
		Serial.print(":");
		Serial.print(w_sec);
		Serial.println();
	#endif

	}

}

void wake_pi(){
	Wire.end(); //to pull A5 low we have to disable i2c first or it will not go low
	pinMode(A5, OUTPUT);  //if runnning a 5v arduino make sure A5 never goes HIGH, as it might damage the 3.3v pi
	digitalWrite(A5, LOW);  //here i pull A5 low, that will wake the pi from halt ("sudo shutdown -h")
	delay(500);
	Wire.begin(SLAVE_ADDRESS); //start the i2c again
}








// callback for received data
void receiveData(int byteCount){

	#ifdef debug
		Serial.print("receiveData: ");
		Serial.print(byteCount);
		Serial.println(" bytes");
	#endif

	if (byteCount == 1){ //if i only receive one byte, thats the register pi want to read next in sendData
		reg = Wire.read();
	}

	if (byteCount >= 2) { //here i receive register and data to write to it
	reg = Wire.read();
	data = Wire.read();

	#ifdef debug
		Serial.print("reg:");
		Serial.println(reg);
		Serial.print("data:");
		Serial.println(data);
	#endif

	switch (reg) { //here i put data in diffrent registers
	    case 1:
	    	w_hour = data;
		    
		    #ifdef debug
		    	Serial.print("hour set to: ");
		    	Serial.println(w_hour);
		    #endif
	    	
	    	break;
	    case 2:
	    	w_min = constrain(data,0,59); //an hour never have more then 60 secounds
	    	
	    	#ifdef debug
	    		Serial.print("min set to: ");
	    		Serial.println(w_min);
	    	#endif

	    	break;
	    case 3:
	    	w_sec = constrain(data,0,59); //a minute never have more then 60 secounds
	     	
	     	#ifdef debug
	     		Serial.print("sec set to: ");
	     		Serial.println(w_sec);
	     	#endif

	     	break;
	    case 4:
	   		w_enable = constrain(data,0,1); //the timer can be on or off
	    	
	    	#ifdef debug
	    		Serial.print("timer enable set to: ");
	     		Serial.println(w_enable);
	    	#endif

	    	break;

	    default: 

	    	#ifdef debug
				Serial.println("register not avalible");
			#endif
	    	
	    	break;
	}


} 

	
while(Wire.available()){ //here i just purge incoming if there is something left, perhaps not needed
	Wire.read();
}
	



}

void sendData(){ //here pi reads data, what register it read is the one it received when it only got one byte in receiveData

	#ifdef debug
		Serial.println("read data");
	#endif


	switch (reg) {
	    case 1:
	    	Wire.write(w_hour);
		
			#ifdef debug
				Serial.print("read hour: ");
				Serial.println(w_hour);
			#endif

	    	break;
	    case 2:
	    	Wire.write(w_min);
	    
	    	#ifdef debug
				Serial.print("read min: ");
				Serial.println(w_min);
			#endif
	    
	    	break;
	    case 3:
	    	Wire.write(w_sec);
	    	
	    	#ifdef debug
				Serial.print("read sec: ");
				Serial.println(w_sec);
			#endif

	    	break;
	    case 4:
	    	Wire.write(int(w_enable));
	    	
	    	#ifdef debug
				Serial.print("read timer enable: ");
				Serial.println(w_enable);
			#endif
	    	
	    	break;

	    default: 

	    	#ifdef debug
				Serial.println("register not avalible");
			#endif
	    	
	    	break;
	}

	Wire.flush(); //here i do a flush, perhaps not needed 

}
