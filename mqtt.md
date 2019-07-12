## Installation
### Server

pip install paho-mqtt

### Client



## Usage
MQTT messages are organized into topics. A topic can be of the form "X/Y", where "X" is some category and "Y" is a particular topic in the category.
+ "Wildcard" topics are allowed, such as "sws007/#"
+ Long topic is possible, such as "X/Y/Z"


MQTT can only handle byte streams, strings, integers and floats.

Example:
+ <userid>/IMAGE/classify: Binary stream of image data
+ <userid>/IMAGE/predict: A string of the form "<class name>:<probability>:<index>". So if we classified tulips with a probability of 0.98, and tulips has an index
of 4 (i.e. it is the 5th class in our training data), we get back: `tulips:0.98:4`
