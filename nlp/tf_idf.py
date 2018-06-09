'''
    1. Put many sentences to the class object
    2.  Tokenize the sentences to the words
    3.  FreqDist this words
    4. Calculate the tf and idf
'''
import nltk
import string

class Collection:
    def __init__(self,sentences):
        self.words_list=[self._tokenize(sentence) for sentence in sentences]
        self.freq_dict=[self._freqdist(words) for words in self.words_list]

    def _tokenize(self,sentence):
        # remove the punctuation
        remove_punctuation_map=dict((ord(char),None) for char in string.punctuation)
        sentence_no_punctuation=sentence.translate(remove_punctuation_map)
        # lower
        sentence_no_punctuation=sentence_no_punctuation.lower()
        # word_tokenize
        words=nltk.word_tokenize(sentence_no_punctuation)
        # remove stopwords
        from nltk.corpus import stopwords
        filtered_words = [word for word in words if word not in stopwords.words('english')]
        # stem
        from nltk.stem import SnowballStemmer
        snowball_stemmer = SnowballStemmer("english")
        words_stemed=[snowball_stemmer.stem(word) for word in filtered_words]
        return words_stemed

    def _freqdist(self,words):
        from nltk import FreqDist
        fdist = FreqDist(words)
        standard_freq_vector = fdist.most_common(50)
        return dict(standard_freq_vector)

    def tf(self,word,sentence):
        word=self._tokenize(word)[0]
        words=self._tokenize(sentence)
        return (sum(1 for word1 in words if word1==word))/len(words)

    def idf(self,word):
        word=self._tokenize(word)[0]
        import math
        try:
            return math.log(len(self.words_list)/(1+sum(1 for words in self.words_list if word in words)))
        except ValueError:
            return 0

    def tf_idf(self,word,sentence):
        return self.tf(word,sentence)*self.idf(word)



text1 = "Natural language processing (NLP) is a field of computer science, artificial intelligence and computational linguistics concerned with the interactions between computers and human (natural) languages, and, in particular, concerned with programming computers to fruitfully process large natural language corpora. Challenges in natural language processing frequently involve natural language understanding, natural language generation (frequently from formal, machine-readable logical forms), connecting language and machine perception, managing human-computer dialog systems, or some combination thereof."

text2 = "The Georgetown experiment in 1954 involved fully automatic translation of more than sixty Russian sentences into English. The authors claimed that within three or five years, machine translation would be a solved problem.[2] However, real progress was much slower, and after the ALPAC report in 1966, which found that ten-year-long research had failed to fulfill the expectations, funding for machine translation was dramatically reduced. Little further research in machine translation was conducted until the late 1980s, when the first statistical machine translation systems were developed."

text3 = "During the 1970s, many programmers began to write conceptual ontologies, which structured real-world information into computer-understandable data. Examples are MARGIE (Schank, 1975), SAM (Cullingford, 1978), PAM (Wilensky, 1978), TaleSpin (Meehan, 1976), QUALM (Lehnert, 1977), Politics (Carbonell, 1979), and Plot Units (Lehnert 1981). During this time, many chatterbots were written including PARRY, Racter, and Jabberwacky。"

collection=Collection([text1,text2,text3])
res=collection.tf_idf('language',text1)
print(res)