import paho.mqtt.client as mqtt
import numpy as np
from PIL import Image


USERID = "sws007"
PASSWORD = "bengalcat"

TMP_FILE = '/tmp/%s.jpg'%(USERID)
dict = {0:'daisy', 1:'dandelion', 2:'roses', 3: 'sunflowers', 4:'tulips'}

resp_callback = None

def load_image(image):
    img = Image.open(image)
    img = img.resize((249, 249))
    imgarray = np.array(img) / 255.0
    final = np.expand_dims(imgarray, axis=0)
    return final

def classify(imgarray, dict):
    return dict[4], 0.98, 4

def on_connect(client, userdata, flags, rc):
    print("Connected with result code %d."%(rc))
    client.subscribe(USERID + "/IMAGE/classify")

def on_message(client, userdata, msg):
    # The payload should contain the image in numpy matrix format
    print('Received message from server. Writing to %s.'%(TMP_FILE))
    

    img = msg.payload
    
    with open(TMP_FILE, 'wb') as f:
        f.write(img)
    imgarray = load_image(TMP_FILE)

    label, prob, index = classify(imgarray, dict)

    print('Classified as %s with certaincy %3.4f.'%(label, prob))
    
    client.publish(USERID + '/IMAGE/predict', label + ':' + str(prob) + ':' + str(index))

        
def setup():
    global dict
    global client
    
    client = mqtt.Client(transport='websockets') # default is tcp
    client.username_pw_set(USERID, PASSWORD) # if it is wrong, we will get an error code of "5"
    client.on_connect = on_connect # function to call on connecting
    client.on_message = on_message # function to call on receiving a message

    print('Connecting')
    x = client.connect('pi.smbox.co', 80, 30) # connect to our MQTT broker
    client.loop_start() # ask the client to start monitoring for responses
    

def main():
    setup()
    while True:
        pass
    
if __name__ == '__main__':
    main()

