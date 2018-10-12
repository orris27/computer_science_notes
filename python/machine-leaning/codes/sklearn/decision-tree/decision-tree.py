'''
    Usage: python decision-tree.py
'''
import csv
from sklearn import preprocessing
from sklearn.feature_extraction import DictVectorizer
from sklearn import tree

# create training_features
# create training_labels
reader = csv.reader(open('AllElectronics.csv','r'))
headers = next(reader)

features = []
labels = []


for line in reader:
    labels.append(line[len(line) - 1])
    # construct a dict: {key: value}
    dictionary = {}
    for index, key in enumerate(headers[1:-1]):
        dictionary[key] = line[index + 1]
    features.append(dictionary)

print(features)
print(labels)
     
# one-hot encoding

vec = DictVectorizer()
features = vec.fit_transform(features).toarray()


lb = preprocessing.LabelBinarizer()
labels = lb.fit_transform(labels)
        

print(features)
print(labels)





# init a decision tree clf
clf = tree.DecisionTreeClassifier(criterion="entropy")
# fit clf with training data
clf = clf.fit(features,labels)

# predict
y_predicted = clf.predict(features)
print(y_predicted)
