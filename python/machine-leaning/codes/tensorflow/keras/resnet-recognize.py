'''
    Usage: put "elephant.jpg" in the current directory & python resnet-recognize.py
'''
import keras
from keras.applications.resnet50 import ResNet50
#from keras.preprocessing import image
from keras.applications.resnet50 import preprocess_input, decode_predictions
import numpy as np

model = ResNet50(weights='imagenet')

filename = 'elephant.jpg'
img = keras.preprocessing.image.load_img(filename, target_size=(224, 224))
image = keras.preprocessing.image.img_to_array(img)
images = np.expand_dims(image, axis=0)
dimages = preprocess_input(images)

y_predicted = model.predict(dimages)
# decode the results into a list of tuples (class, description, probability)
# (one such list for each sample in the batch)
print('Predicted:', decode_predictions(y_predicted, top=3)[0])
# Predicted: [(u'n02504013', u'Indian_elephant', 0.82658225), (u'n01871265', u'tusker', 0.1122357), (u'n02504458', u'African_elephant', 0.061040461)]

