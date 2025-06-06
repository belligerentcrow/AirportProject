# Airport Project
Work in progress!

## The Plan
* A thread checks positions of planes
	* It has to figure out if two or more are within 500m of each other
		* If so, the control tower sends a message to both, alerting them
			* The planes should change course in order to put distance between each other
	* If a plane is out of a certain range from the control tower, then it disconnects from it 
		* Presumably it will in its journey connects to another one
	* Therefore, control tower must have a Lat and a Long (its own position)
* **control tower**: `thread receiving` airplane data and-or messages
	* Should it broadcast it to all other planes? 
	* Maybe all-time broadcast is too much. It could be a broadcast mode which could be activated in the "danger-zone" areas (when planes are either on the ground or close)
* **control tower**: `thread sending` airplane messages and-or commands 
* **control tower**: `thread checking` position of planes, checking if plane positions are dangerously close to each other
	* Must access latter plane data in a mutually exclusive way
* **plane**: `thread sending` airplane data to the control tower
* **plane**: `thread receiving` messages and commands from the ground

### Later features, maybe
* *Later*: might have the user select direction of plane movement
* *Later*: might have a control tower signal to another control tower the fact that there's a plane headed there, which is departing from the current control tower
	* Server-to-Server connection (TCP)
* *Later*: plane asks for roger to land in the airport 