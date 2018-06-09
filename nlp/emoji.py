import re
emoticons_str=r"""
    (?:
        [:=;]
        [oO\-]?
        [D\)\]\(\]/\\OpP]
    )"""
regex_str=[
        emoticons_str,
        r'<[^>]+>', # html tags
        r'(?:@[\w_]+)', # @ sb.
        r'(?:\#+[\w_]+[\w\'_\-]*[\w_]+)', #topic tags
        r'http[s]?://(?:[a-z]|[0-9]|[$-_@.&amp;+]|[!*\(\),]|(?:%[0-9a-f][0-9a-f]))+', # url
        r'(?:(?:\d+,?)+(?:\.?\d+)?)', # number
        r"(?:[a-z][a-z'\-_]+[a-z])", # words that contains - and '
        r'(?:[\w_]+)', # etc
        r'(?:\S)', # etc
        ]
            
tokens_re=re.compile(r'('+'|'.join(regex_str)+')',re.VERBOSE|re.IGNORECASE)
emoticon_re=re.compile(r'^'+emoticons_str+'$',re.VERBOSE|re.IGNORECASE)

def tokenize(s):
    return tokens_re.findall(s)

def preprocess(s,lowercase=False):
    tokens=tokenize(s)
    if lowercase:
        tokens=[token if emoticon_re.search(token) else token.lower() for token in tokens]
    return tokens

tweet='RT @angelababy: love you baby! :D http://ah.love #168cm'
print(preprocess(tweet))
            




