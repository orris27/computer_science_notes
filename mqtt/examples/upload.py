#!/usr/bin/env python
# coding: utf-8

# In[1]:


import paho.mqtt.client as mqtt
import time


# In[ ]:


USERID = "sws007"
PASSWORD = "bengalcat"

resp_callback = None

def on_connect(client, userdata, flags, rc):
    print("Connected. Result code is %d."%(rc))
    client.subscribe(USERID + "/IMAGE/predict")

def on_message(client, userdata, msg):
    '''
        msg: contain several fields, including 'topic' and 'payload' which is the actucal data sent by the server
            'payload' is normaly a bytestream, so we need to convert it to a proper string using "msg.payload.decode('utf-8')"
    '''
    print('Received message from server.', msg.payload)
    
    tmp = msg.payload.decode('utf-8')
    str = tmp.split(':')
    
    if resp_callback is not None:
        resp_callback(str[0], float(str[1]), int(str[2]))
        
def setup():
    global client
    
    client = mqtt.Client(transport='websockets') # default is tcp
    client.username_pw_set(USERID, PASSWORD) # if it is wrong, we will get an error code of "5"
    client.on_connect = on_connect # function to call on connecting
    client.on_message = on_message # function to call on receiving a message
    client.connect('pi.smbox.co', 80, 30) # connect to our MQTT broker
    client.loop_start() # ask the client to start monitoring for responses
    
def load_image(filename):
    with open(filename, 'rb') as f:
        data = f.read()
    return data

def send_image(filename):
    img = load_image(filename)
    client.publish(USERID + '/IMAGE/classify', img) # topic & data
    
def resp_handler(label, prob, index):
    print('\n -- Response -- \n\n')
    print('Label: %s'%(label))
    print('Probability: %3.4f'%(prob))
    print('Index: %d'%(index))
    
def main():
    global resp_callback
    
    setup()
    resp_callback = resp_handler
    print('Waiting 2 seconds before sending.')
    time.sleep(2)
    print('Sending tulip.jpg')
    send_image('tulip.jpg')
    print('Done. Waiting 5 seconds before sending')
    time.sleep(5)
    print('Sending tulip2.jpg')
    send_image('tulip2.jpg')
    while True:
        pass
    
if __name__ == '__main__':
    main()

