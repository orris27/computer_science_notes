'''
    python iris.py
'''
from sklearn import datasets
from sklearn import neighbors

iris = datasets.load_iris()

print(iris)

clf = neighbors.KNeighborsClassifier()

clf.fit(iris.data, iris.target)

y_predicted = clf.predict(iris.data)
#y_predicted = clf.predict([[0.1, 0.2, 0.3, 0.4]])
print(y_predicted)
