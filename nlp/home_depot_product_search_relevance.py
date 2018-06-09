import numpy as np
import pandas as pd
import os
import nltk
import string


def tokenize(sentence):
    '''
        clean the product_title + search_term
    '''
    # remove the punctuation
    remove_punctuation_map = dict((ord(char), None) for char in string.punctuation)
    sentence_no_punctuation = sentence.translate(remove_punctuation_map)
    # lower
    sentence_no_punctuation = sentence_no_punctuation.lower()
    # word_tokenize
    words = nltk.word_tokenize(sentence_no_punctuation)
    # remove stopwords
    from nltk.corpus import stopwords
    filtered_words = [word for word in words if word not in stopwords.words('english')]
    # stem
    from nltk.stem import SnowballStemmer
    snowball_stemmer = SnowballStemmer("english")
    words_stemed = [snowball_stemmer.stem(word) for word in filtered_words]
    return words_stemed

def tokenize_string(sentence):
    return ' '.join(tokenize(sentence))



data_dir='kaggle-data'
df_train=pd.read_csv(os.path.join(data_dir,'train.csv'),encoding='ISO-8859-1')
df_test=pd.read_csv(os.path.join(data_dir,'test.csv'),encoding='ISO-8859-1')
df_desc=pd.read_csv(os.path.join(data_dir,'product_descriptions.csv'))

# concat the data_train and data_test
df_all=pd.concat((df_train,df_test),axis=0,ignore_index=True)
# merge the data_product_uid + data_desc + data_product title
df_all=pd.merge(df_all,df_desc,how='left',on='product_uid')
print('processing...')
df_all['search_term']=df_all['search_term'].map(lambda x:tokenize_string(x))
df_all['product_title']=df_all['product_title'].map(lambda x:tokenize_string(x))
df_all['product_description']=df_all['product_description'].map(lambda x:tokenize_string(x))
print('finished!')

# cal the total of terms in the search_term
df_all['search_total']=df_all['search_term'].map(lambda x:len(x.split(' ')))

print('Calculating the common attributes...')
# get the total of common words between the product_title and search_term
def get_common(s1,s2):
    # contains! not exact
    return sum(int(s2.find(word)>=0) for word in s1.split(' '))
df_all['commmon_in_title_total']=df_all.apply(lambda x:get_common(x['search_term'],x['product_title']),axis=1)
# get the total of common words between the product_description and search_term
df_all['commmon_in_desc_total']=df_all.apply(lambda x:get_common(x['search_term'],x['product_description']),axis=1)

print('Finished calculating the common attributes...')


# drop the attribute that the computer cannot understand
df_all=df_all.drop(['search_term','product_title','product_description'],axis=1)

# restore the training data and test data ??the train/test is included in the test/train??
df_train=df_all.loc[df_train.index]
df_test=df_all.loc[df_test.index]

# records ids
df_test_ids=df_test['id']

# get y_train
y_train=df_train['relevance'].values

print("********")# debug
print('y_train',y_train)

# get X_train ??should i drop the product_uid as well??
X_train=df_train.drop(["id","relevance"],axis=1).values

print("********")# debug
print('X_train',X_train)

# get X_test
X_test=df_test.drop(["id",'relevance'],axis=1).values
# print(X_test)

'''machine learning'''
# build a model
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import cross_val_score
# Random Forest ( depth)
params=[1,3,5,6,7,8,9,10] # check for each depth to find out which one performs best
test_scores=[] # get score for each depth (designed to find out which one performs best)
for param in params:
    clf=RandomForestRegressor(n_estimators=30,max_depth=param) # construct a classifier
    test_score=np.sqrt(-cross_val_score(clf,X_train,y_train,scoring='neg_mean_squared_error',cv=5)) # put the training data in and cal the cost
    test_scores.append(np.mean(test_score)) # cal the average cost

# find out which depth is best
import matplotlib.pyplot as plt # draw
plt.figure()
plt.plot(params,test_scores)
plt.title("Param vs CV Error")
plt.show()


rf=RandomForestRegressor(n_estimators=30,max_depth=6)
rf.fit(X_train,y_train)
y_predicted=rf.predict(X_test)

pd.DataFrame({"id":df_test_ids,"relevance":y_predicted}).to_csv("submission.csv",index=False)