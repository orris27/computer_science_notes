## Usage
### Run
1. Install python package and server
```
pip install paho-mqtt
```
2. Prepare `tulip.jpg` and `tulip2.jpg`. For example:
```
wget -O tulip.jpg https://images.homedepot-static.com/productImages/71c23308-8d69-46bd-b9d3-af5a40c6e45c/svn/garden-state-bulb-flower-bulbs-hof18-05-64_1000.jpg
wget -O tulip2.jpg https://www.woodenshoe.com/media/attila-graffiti-tulip.jpg
```


3. Run the codes
```
python classify.py # run on server

python upload.py # run on client pc
```
### Outputs
+ classify.py
```
Connecting
Connected with result code 0.
Received message from server. Writing to /tmp/sws007.jpg.
Classified as tulips with certaincy 0.9800.
Received message from server. Writing to /tmp/sws007.jpg.
Classified as tulips with certaincy 0.9800.
```
+ upload.py
```
Waiting 2 seconds before sending.
Connected. Result code is 0.
Sending tulip.jpg
Done. Waiting 5 seconds before sending
Received message from server. b'tulips:0.98:4'

 -- Response -- 


Label: tulips
Probability: 0.9800
Index: 4
Sending tulip2.jpg
Received message from server. b'tulips:0.98:4'

 -- Response -- 


Label: tulips
Probability: 0.9800
Index: 4
```
