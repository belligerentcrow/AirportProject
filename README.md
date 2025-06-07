# Airport Project

* Both server and clients need to be compiled with the -lm linked library (due to the cos and sin functions)
  * `gcc ./src/serverControlTower.c -lm -o ./server`
  * `gcc ./src/clientPlane.c -lm -o ./clientPlane`
* usage example for the server: `./serverTower 8080 ABCD`
* usage example for the clients: `./clientPlane 127.0.0.1 8080 XYZRPG ROME LNDN`
* most functionalities are a work in progress!

## The Plan
* A thread checks positions of planes
	* It has to figure out if two or more are within 500m of each other
		* If so, the control tower sends a message to both, alerting them
			* The planes should change course in order to put distance between each other
	* If a plane is out of a certain range from the control tower, then it disconnects from it 
		* *(Presumably it will, in its journey, connect to another one)*
	* Therefore, control tower must have a Lat and a Long (its own position)
* **control tower**: `thread receiving` airplane data and-or messages
	* ControlTower can receive a message which can either be interpreted as DATA or as MESSAGE depending on a `TYPE_PAYLOAD enum`.
		* Both DATA or ALERT payloads should be 64Bytes, which makes the whole thing work with `memcpy()`
			* Assigning the memory to the correct struct when identifying the type
	* Should it broadcast data / alerts to all other planes? 
	* Maybe all-time broadcast is too much. It could be a broadcast mode which could be activated in the "danger-zone" areas (when planes are either on the ground or close)
		* Implies the existence of a "DangerMode" where all planes go crazy receiving many alerts... will think this through, or get inspiration from actual real-life control tower dynamics perhaps  
* **control tower**: `thread sending` airplane messages and-or commands 
* **control tower**: `thread checking` position of planes, checking if plane positions are dangerously close to each other
	* Must access latter plane data in a mutually exclusive way
* **plane**: `thread sending` airplane data to the control tower
* **plane**: `thread receiving` messages and commands from the ground
* Easter egg...

### Later features, maybe
* *Later*: might have the user select direction of plane movement
* *Later*: might have a control tower signal to another control tower the fact that there's a plane headed there, which is departing from the current control tower
	* Server-to-Server connection (TCP)
* *Later*: plane asks for roger to land in the airport 