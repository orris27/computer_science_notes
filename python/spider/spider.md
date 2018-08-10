## 1. Spider
### 1-1. 语法
1. Request对象
    1. 库   
        1. `urllib.request`
    2. 内容
        1. URL
        2. 请求头
            1. 用户代理
                + 构造函数里指定
                + 也可以后期添加
            2. Cookie
        3. 请求体
            1. 数据
                + 尤其是post的时候
                + `urlencode(字典).encode(utf-8)`
                + bytes类型
                + 不能出现中文,使用url编码
    3. 发送请求
        1. Request对象
        2. 发送方式
            1. urlopen
            2. opener对象
        3. context
            1. HTTPS: `context=ssl._create_unverified_context()`
            
2. opener对象
    1. handler对象
        1. ProxyHandler
            + 使用代理IP地址
        2. HTTPCookieProcessor
            1. 需要配合CookieJar对象
3. respone
    > urlopen或者opener.open的返回值,包含响应头和响应体
    1. 状态码: `response.getcode()`
    2. 请求的URL: `response.geturl()`
    3. 响应体: `response.read()`(bytes类型,转换的话需要decode)
        1. 字符串: `response.read().decode('utf-8')`
        2. 处理成JSON: `json.loads(response.read().decode('utf-8'))`
            > bytes=>表示JSON的字符串=>Python的字典
            
```
handler=ProxyHandler({"http":"222.222.169.60:53281"})
opener=build_opener(handler)
response=opener.open(request)
```



## 2. 数据提取
### 2-1. re模块
> https://github.com/orris27/orris/blob/master/python/re/re.md

### 2-2. JSONPath
处理JSON. 响应体=bytes => 表示JSON的字符串 => Python的字典对象
+ 最终得到的数据类型是list
```
jn=response.read().decode('utf-8')
jn=json.loads(jn)
city_list=jsonpath.jsonpath(jn,'$..A[1:4].name')
for city in city_list:
    print(city)
```

### 2-3. etree + XPath
通过html标签的层级关系提取数据.响应体bytes => 字符串 => dom => 数据列表(xpath处理后一定是个列表对象)
+ 最终得到的数据类型是list
```
#response=urlopen(...)
html=response.read().decode("utf-8")
dom=etree.HTML(html,parser=etree.HTMLParser(encoding='utf-8'))
data_list=dom.xpath('//div[@class="nav-con fl"]/ul/li/a/@href')
for data in data_list:
    #....
```


## 3. 实例
1. 使用随机用户代理
    1. 导入request库和random库
    2. 利用URL构造Request对象
    3. 给这个Request对象添加用户代理
        1. 设定用户代理池
        2. 从用户代理池随机抽取一个赋值到请求体上
        
```
from urllib.request import *
import random
url="http://www.renren.com/"
request=Request(url)

ua_list=[
    "Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.122 Safari/537.36 SE 2.X MetaSr 1.0",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.87 Safari/537.36 OPR/37.0.2178.32,",
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
]
user_agent=random.choice(ua_list)


request.add_header('User-Agent',user_agent)

print(request.get_header('User-agent'))

```


2. 使用随机代理搜索百度关键词itcast,将结果写到html文件里
    1. 构造Request对象
        1. URL
            1. URL地址
            2. GET参数
        2. 用户代理
    2. 发送request请求
        + request请求就是Request对象
        + 使用urlopen打开bianm
    3. 将响应写入到html文件里
        1. 响应用utf8解码,得到python3的字符串类型
            + 网络中传输的信息在python3里是bytes类型
        2. 将字符串类型写入到文件里
        
```
from urllib.request import *
from urllib.parse import *
import random

# https://www.baidu.com/s?wd=传智播客
url="https://www.baidu.com/s?"

wd=urlencode({"wd":"传智播客"})
url+=wd

# print(url)


ua_list=[
    "Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.122 Safari/537.36 SE 2.X MetaSr 1.0",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.87 Safari/537.36 OPR/37.0.2178.32,",
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
]
useragent=random.choice(ua_list)
headers={"User-Agent":useragent}
request=Request(url,headers=headers)

response=urlopen(request)
# print(type(response))
# print(response.read().decode('utf-8'))

with open('itcast.html','w') as f:
    f.write(str(response.read().decode('utf-8')))

```
3. 遍历百度贴吧的若干页获取网页源码
    1. 构造Request对象
        1. URL
            1. 贴吧名
            2. 哪些页(循环遍历)
                1. 起始页
                2. 结束页
        2. 用户代理
    2. 发送request请求
    3. 将响应体写入带指定文件里
```
'''
    直接"python xx.py"输入就行了.后面有问答形式.最终输出到当前文件夹的"贴吧名-页数.html"
'''
from urllib.request import *
from urllib.parse import  *
import random

if __name__ == "__main__":
    url="http://tieba.baidu.com/f?"
    name=input("请输入贴吧名:")
    kw=urlencode({"kw":name})
    url+=kw
    spage=int(input("起始页:"))
    epage=int(input("结束页:"))

    ua_list = [
        "Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko",
        "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.122 Safari/537.36 SE 2.X MetaSr 1.0",
        "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.87 Safari/537.36 OPR/37.0.2178.32,",
        "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
    ]
    useragent = random.choice(ua_list)
    headers = {"User-Agent": useragent}


    for i in range(spage,epage+1):
        page=(i-1)*50
        pn=urlencode({"pn":str(page)})
        fullurl=url+"&"+pn
        request = Request(fullurl, headers=headers)

        print("正在加载第"+str(i)+"页...")

        response = urlopen(request)

        filename=name+"-"+str(i)+ "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.87 Safari/537.36 OPR/37.0.2178.32,",".html"
        with open(filename,"w") as f:
            f.write(response.read().decode('utf-8'))
        print("写入成功")
    print("谢谢使用")

```


4. post请求字典API,并返回结果
    1. 构造Request对象  
        1. URL
        2. 请求头
            1. 用户代理
        3. 请求体
            1. post数据
                + 经过url编码并且转换为bytes类型
    2. 发送request请求
    3. 输出响应
```
from urllib.request import *
from urllib.parse import *

# POST请求的目标URL
url = "http://fanyi.youdao.com/translate?smartresult=dict&smartresult=rule&smartresult=ugc&sessionFrom=null"

headers={"User-Agent":  "Mozilla/5.0 (duixiangWindows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.87 Safari/537.36 OPR/37.0.2178.32,"}

formdata = {
    "type":"AUTO",
    "i":"i love python",
    "doctype":"json",
    "xmlVersion":"1.8",
    "keyfrom":"fanyi.web",
    "ue":"UTF-8",
    "action":"FY_BY_ENTER",
    "typoResult":"true"
}

data = urlencode(formdata).encode('utf-8')
formdata = {
    "type":"AUTO",


request = Request(url, data = data, headers = headers)
response = urlopen(request)
print (response.read().decode('utf-8'))
```

5. Cookie
    1. 构造Request对象  
        1. URL
        2. 请求头
            1. 用户代理
            2. Cookie
    2. 发送request请求
    3. 输出响应
```
from urllib.request import *
from urllib.parse import *

url="http://i.baidu.com/"

headers={
"Accept":"text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8",
"Accept-Language":"en-US,en;q=0.9,zh-CN;q=0.8,zh;q=0.7",
# "Cookie":"BAIDUID=6FB3B3BFEBD5DC4DC647104DB387829A:FG=1; BIDUPSID=6FB3B3BFEBD5DC4DC647104DB387829A; PSTM=1523610784; BDUSS=zRyeThpSjJ4b2hUU2UzUXdVYU5pM01YbVdJWTdabUl3fndZT2lXZzVoVn5EdmxhQVFBQUFBJCQAAAAAAAAAAAEAAAD4~nKpUDRKdXN0aWNSYWluAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH-B0Vp~gdFaN; BDORZ=B490B5EBF6F3CD402E515D22BCDA1598; locale=zh; H_PS_PSSID=1446_21123_20930; PSINO=7; PHPSESSID=dikhg9imc1u3a90mi1tpuljii7; Hm_lvt_4010fd5075fcfe46a16ec4cb65e02f04=1525776325; Hm_lpvt_4010fd5075fcfe46a16ec4cb65e02f04=1525776325",
"Host":"i.baidu.com",
"Upgrade-Insecure-Requests":"1",
"User-Agent":"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
}

request=Request(url,headers=headers)

response=urlopen(request)

with open('html/ibaidu_nocookie.html','w') as f:
    f.write(response.read().decode('utf-8'))
```


6. 使用代理IP爬虫
    1. 构造Request对象
    2. 构造opener对象(浏览器)
        1. 使用的代理IP
    3. 用该"浏览器"发出request请求
    4. 输出响应
```
from urllib.request import *
from urllib.parse import *

url="http://www.baidu.com/"

headers={
"User-Agent":"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
}

request=Request(url,headers=headers)

handler=ProxyHandler({"http":"222.222.169.60:53281"})

opener=build_opener(handler)
response=opener.open(request)

with open('../html/handler2.html','w') as f:
    f.write(response.read().decode('utf-8'))
```


7. 使用CookieJar来保存cookie信息
    > 和直接使用cookie不同,CookieJar是需要先请求后才会将里面的cookie保存起来.也就是说,先登录之后才能拿到Cookie
    1. 构造Request对象
        1. URL
        2. 请求头
        3. 请求体
    2. 利用CookieJar和HTTPCookieProcessor构造opener("浏览器")
    3. 利用该浏览器先发送第一次request请求,获得Cookie
    4. 再利用原浏览器发送第二次request请求,这次浏览器发送时带有cookie
        + 使用原来的opener
        + 或者将opener注册到全局,这样urlopen会直接使用该opener
    5. 输出响应
```
from urllib.request import *
from urllib.parse import *
from http.cookiejar import CookieJar
import random

##############################################################
# Create an opener
##############################################################
cookiejar=CookieJar()
handler=HTTPCookieProcessor(cookiejar)
opener=build_opener(handler)

ua_list=[
    "Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.122 Safari/537.36 SE 2.X MetaSr 1.0",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.87 Safari/537.36 OPR/37.0.2178.32,",
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
]
user_agent=random.choice(ua_list)

headers={
    'User-Agent':user_agent,
}
url="http://www.renren.com/PLogin.do/"

data={
   "email":"18868107624",
    "password":"jiwumingshi",
}
data=urlencode(data).encode('utf-8')

##############################################################
# 1st Request
##############################################################
request=Request(url,data=data,headers=headers)
response=opener.open(request)


##############################################################
# 2nd Request
##############################################################
install_opener(opener)
request2=Request('http://www.renren.com/965835048/profile')
# response2=urlopen(request2)
response2=opener.open(request2)


with open('../html/test.html','w') as f:
    # f.write(response.read().decode('utf-8'))
    # f.write(response2.read().decode('utf-8'))
    f.write(response2.read().decode('utf-8'))

```


8. 处理https
    > 发送请求的时候说明无视SSL证书
```
from urllib.request import *
from urllib.parse import *
import random
import ssl
context=ssl._create_unverified_context()
url="https://www.12306.cn/mormhweb/"
ua_list=[
    "Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.122 Safari/537.36 SE 2.X MetaSr 1.0",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.87 Safari/537.36 OPR/37.0.2178.32,",
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
]
user_agent=random.choice(ua_list)
headers={
    'User-Agent':user_agent,
}
request=Request(url,headers=headers)

response=urlopen(request,context=context)
with open('https2.html','w') as f:
    f.write(response.read().decode('utf-8'))
```


9. JSONPath处理JSON
    > 响应体=bytes => 表示JSON的字符串 => Python的字典对象
    1. 构造Request请求
    2. 发送请求
    3. 接收响应体
    4. 将响应体转换成python的字典对象
    5. 对字典对象采用JSONPath处理获得想要的数据
    
```
import jsonpath
from urllib.request import *
import json
from urllib.parse import *
url = 'http://www.lagou.com/lbs/getAllCitySearchLabels.json'
headers={
       'User-Agent':"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
}
request=Request(url,headers=headers)
response=urlopen(request)

jn=response.read().decode('utf-8')
jn=json.loads(jn)
city_list=jsonpath.jsonpath(jn,'$..A[1:4].name')
for city in city_list:
    print(city)
```


10. etree和XPath提取数据
    1. 构造Request请求
    2. 发送请求
    3. 接收响应
    4. 通过响应获得dom,再读dom进行xpath处理提取数据
        > 响应体bytes => 字符串 => dom => 数据列表(xpath处理后一定是个列表对象)
        ```
        html=response.read().decode("utf-8")
        dom=etree.HTML(html,parser=etree.HTMLParser(encoding='utf-8'))
        data_list=dom.xpath('//div[@class="nav-con fl"]/ul/li/a/@href')
        for data in data_list:
            #....
        ```
```
from urllib.request import *
from urllib.parse import *
from lxml import etree

url="http://www.bilibili.com"
headers={
    'User-Agent':"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36",
}
request=Request(url,headers=headers)
response=urlopen(request)
html=response.read().decode("utf-8")
dom=etree.HTML(html,parser=etree.HTMLParser(encoding='utf-8'))
with open('pic.html','w') as f:
    f.write(etree.tostring(dom).decode('utf-8'))
link_list=dom.xpath('//div[@class="nav-con fl"]/ul/li/a/@href')

print(link_list)
for link in link_list:
    print(link)
```

## 4. 常用代码
1. user-agent
```
ua_list=[
    "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Win64; x64; Trident/5.0; .NET CLR 3.5.30729; .NET CLR 3.0.30729; .NET CLR 2.0.50727; Media Center PC 6.0)",
    "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; .NET CLR 1.0.3705; .NET CLR 1.1.4322)",
    "Mozilla/4.0 (compatible; MSIE 7.0b; Windows NT 5.2; .NET CLR 1.1.4322; .NET CLR 2.0.50727; InfoPath.2; .NET CLR 3.0.04506.30)",
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN) AppleWebKit/523.15 (KHTML, like Gecko, Safari/419.3) Arora/0.3 (Change: 287 c9dfb30)",
    "Mozilla/5.0 (X11; U; Linux; en-US) AppleWebKit/527+ (KHTML, like Gecko, Safari/419.3) Arora/0.6",
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.2pre) Gecko/20070215 K-Ninja/2.1.1",
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9) Gecko/20080705 Firefox/3.0 Kapiko/3.0",
    "Mozilla/5.0 (X11; Linux i686; U;) Gecko/20070322 Kazehakase/0.4.5"
    'DYZB/2.290 (iPhone; iOS 9.3.4; Scale/2.00)',
]
user_agent=random.choice(ua_list)
headers = {"User-Agent": user_agent}
```
2. sublime替换成字典
```
^(.*)=\s(.*)$  
"\1":"\2",
```

3. post请求
```
url = "http://fanyi.youdao.com/translate?smartresult=dict&smartresult=rule&smartresult=ugc&sessionFrom=null"
headers={"User-Agent": "Mozilla...."}
formdata = {
    "type":"AUTO",
    "i":"i love python",
    "doctype":"json",
    "xmlVersion":"1.8",
    "keyfrom":"fanyi.web",
    "ue":"UTF-8",
    "action":"FY_BY_ENTER",
    "typoResult":"true"
}

data = urlencode(formdata).encode('utf-8')

request = Request(url, data = data, headers = headers)
```

4. 中文处理
    1. url(url+uri+参数)里出现中文要做编码处理
```
from urllib.request import *
from urllib.parse import *

wd=urlencode({"wd":"传智播客"})

print(wd)
```


## 0. 注意

1. json.dumps()
默认为ascii编码

```
print json.dumps(dictStr, ensure_ascii=False) 
chardet.detect(json.dumps(dictStr, ensure_ascii=False))
```



## 5. scrapy



### 3-1. middlewares.py

1. user-agent
    > 放到settings.py文件内, 或者任何地方.

```
　　USER_AGENTS = [
    "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Win64; x64; Trident/5.0; .NET CLR 3.5.30729; .NET CLR 3.0.30729; .NET CLR 2.0.50727; Media Center PC 6.0)",
    "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; .NET CLR 1.0.3705; .NET CLR 1.1.4322)",
    "Mozilla/4.0 (compatible; MSIE 7.0b; Windows NT 5.2; .NET CLR 1.1.4322; .NET CLR 2.0.50727; InfoPath.2; .NET CLR 3.0.04506.30)",
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN) AppleWebKit/523.15 (KHTML, like Gecko, Safari/419.3) Arora/0.3 (Change: 287 c9dfb30)",
    "Mozilla/5.0 (X11; U; Linux; en-US) AppleWebKit/527+ (KHTML, like Gecko, Safari/419.3) Arora/0.6",
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.2pre) Gecko/20070215 K-Ninja/2.1.1",
    "Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9) Gecko/20080705 Firefox/3.0 Kapiko/3.0",
    "Mozilla/5.0 (X11; Linux i686; U;) Gecko/20070322 Kazehakase/0.4.5"
    ]
```

```
from scrapy import signals
from scrapy.conf import settings
import random
import base64

class RandomUserAgent(object):
    def process_request(self,request,spider):
        user_agent=random.choice(settings['USER_AGENTS'])
        request.headers.setdefault('User-Agent',user_agent)
        return None
```







### 3-2. 代理ip( proxy)
#### 3-2-1. 免费短期代理网站举例
可以在线检测代理ip!
- [西刺免费代理IP](http://www.xicidaili.com/)
- [快代理免费代理](http://www.kuaidaili.com/free/inha/)
- [Proxy360代理](http://www.proxy360.cn/default.aspx)
- [全网代理IP](http://www.goubanjia.com/free/index.shtml)

#### 3-2-2. 使用
```
PROXIES=[
    #{"ip_port":"113.200.159.155:9999"},        
    #{"ip_port":"115.218.219.91:9000"},
    {"ip_port":"120.24.171.107:16818","user_pwd":"993790240:cmvfksja"},       
]
```

```
from scrapy import signals
from scrapy.conf import settings
import random
import base64


class RandomProxy(object):
    def process_request(self,request,spider):
        proxy=random.choice(settings['PROXIES'])
        user_pwd=proxy.get('user_pwd')
        if user_pwd is None:
            request.meta['proxy']='http://'+proxy['ip_port']
        else:
            user_pwd_64=base64.b64encode(user_pwd.encode("ascii"))
            request.headers['Proxy-Authorization']='Basic '+user_pwd_64.decode("ascii")
            request.meta['proxy']='http://'+proxy['ip_port']
        return None

```

注意:

1. Proxy-Authorization不要写错
2. 'Basic '大小写和空格不要弄错



### 3-3. xpath
+ 使用ie的user-agent会比较好
+ 作用在response对象上
~~response.body~~

### 3-4. process_item

- 一定要```return item```
- close_spider 要返回什么东西吗


### pipeline

- 如果在爬虫的parse函数中写的是items.py中的类,那么要在pipeline中进行dict()转换
- 保存图片:pipeline

```
from scrapy.pipelines.images import ImagesPipeline
```



### settings.py

在settings.py中设置的变量可以通过下面的方法来获取

```python
from scrapy.utils.project import get_project_settings
xxx=get_project_settings().get('IMAGES_STORE')
```

```python
from scrapy.conf import settings
host = settings['MONGODB_HOST']
```



### 存储图片



```python
ITEM_PIPELINES = {'douyuSpider.pipelines.ImagesPipeline': 1}

# Images 的存放位置，之后会在pipelines.py里调用
IMAGES_STORE = "/Users/Power/lesson_python/douyuSpider/Images"

# user-agent
USER_AGENT = 'DYZB/2.290 (iPhone; iOS 9.3.4; Scale/2.00)'
```



```python
import scrapy
import os
from scrapy.pipelines.images import ImagesPipeline
from scrapy.utils.project import get_project_settings

class ImagesPipeline(ImagesPipeline):
    IMAGES_STORE = get_project_settings().get("IMAGES_STORE")

    def get_media_requests(self, item, info):
        image_url = item["imagesUrls"]
        yield scrapy.Request(image_url)

    def item_completed(self, results, item, info):
        # 固定写法，获取图片路径，同时判断这个路径是否正确，如果正确，就放到 image_path里，ImagesPipeline源码剖析可见
        image_path = [x["path"] for ok, x in results if ok]

        os.rename(self.IMAGES_STORE + "/" + image_path[0], self.IMAGES_STORE + "/" + item["name"] + ".jpg")
        item["imagesPath"] = self.IMAGES_STORE + "/" + item["name"]

        return item

#get_media_requests的作用就是为每一个图片链接生成一个Request对象，这个方法的输出将作为item_completed的输入中的results，results是一个元组，每个元组包括(success, imageinfoorfailure)。如果success=true，imageinfoor_failure是一个字典，包括url/path/checksum三个key。
```

#### 重点

- 一定要设置IMAGE_STORE
- 类以及方法要严格相同
- 注意get_media_requests为yield,而item_completed为return
- 注意第一个方法为meida, 第二个方法为item
- image_path返回的是列表
- IMAGES_STORE为图片存储的位置
- item为parse传输过来的对象

### 编码

```python
# -*- coding: utf-8 -*-
```

### &nbsp;

用print处理后,复制该空格! 因为一般的space键与这个空格不一样

scrapy shell中 print的结果和直接回车的结果不一样.



### response

- 如果是标签的话,那么就用xpath
- 如果是json的话,那么就用response.text

### sublime替换成字典

```
^(.*)=\s(.*)$  
"\1":"\2",
```

### 创建文件夹

```python
if(not os.path.exists(parentFolder)):
    os.makedirs(parentFolder)
```



# debug

### object of type 'NoneType' has no len()

```python
class RandomUserAgent(object):
    def process_request(self,request,spider):
        user_agent=random.choice(settings['USER_AGENTS'])
        print(user_agent)
        return None
```

上面的settings['USER_AGENTS']被我写成了'USER-AGENTS'




## 3. Scrapy
> [传智博客Scrapy课件](http://www.chinahufei.com/notes/Spider/file/part04/4.html)
### 3-1. 实践
1. 参考列表
    1. 最基础的一个项目:[MySpider](https://github.com/orris27/orris/tree/master/python/spider/scrapy_examples/mySpider)
    2. 使用普通爬虫做多页爬取:[dongguan](https://github.com/orris27/orris/tree/master/python/spider/scrapy_examples/dongguan)
    3. 最简单的一个crawlSpider:[tencent1](https://github.com/orris27/orris/tree/master/python/spider/scrapy_examples/tencent1)
    
    
    
    
    
    
2. 创建1个Scrapy项目,获得传智播客的老师名单,并写入到`teacher.json`文件中
    1. 创建scrapy项目
    2. 在`items.py`中写下自己的心愿单
        + 自己想要抓取什么样的数据
    3. 创建1个爬虫
    4. 告诉这个爬虫要怎样才能爬取到我想要的东西
        1. 在哪里爬
        2. 不要爬出去
        3. 获得的响应怎么预先处理
            + extract出来的结果是utf8,如果直接return并写入到json文件的话,是显示`\uxxxx`的形式,但是如果使用yield的话,我们后来在pipelines中decode('utf8')就可以转换成正常的字符串了
        4. 是return好呢还是yield给pipelines做数据收集呢
    5. return/yield
        1. 如果是return的话,直接让爬虫爬,并指定文件名,通过扩展名来确定存储方式
        2. 如果是yield的话,编辑`pipelines.py`文件处理传过来的items(本质是处理这个"字典"数据),并告诉Scrapy要使用这个pipelines
            1. 在pipelines中说明要怎么存储已经被spider处理好的数据
                1. 存储到文件:items类型的数据(基本可以当python字典用) => JSON字符串 => 文件
                2. 存储到数据库:调用对应数据库的接口实现
            2. 在`settings.py`里告诉Scrapy我们要用这个pipelines
```
pip install --upgrade pip
sudo apt-get install python-dev python-pip libxml2-dev libxslt1-dev zlib1g-dev libffi-dev libssl-dev
sudo pip install scrapy

scrapy # 如果有输出就说明安装成功了

scrapy startproject mySpider
#-----------------------------------------------
# ...
# You can start your first spider with:
#     cd mySpider
#     scrapy genspider example example.com
#-----------------------------------------------
tree .
#-----------------------------------------------
#.
#└── mySpider
#    ├── mySpider
#    │   ├── __init__.py
#    │   ├── items.py
#    │   ├── middlewares.py
#    │   ├── pipelines.py
#    │   ├── __pycache__
#    │   ├── settings.py
#    │   └── spiders
#    │       ├── __init__.py
#    │       └── __pycache__
#    └── scrapy.cfg
#-----------------------------------------------


cd mySpider/mySpider/
vim items.py
import scrapy

############################################################
class MyspiderItem(scrapy.Item):
    name = scrapy.Field()
    level = scrapy.Field()
    info = scrapy.Field()
############################################################

# 在当前目录下输入命令，将在mySpider/spider目录下创建一个名为itcast的爬虫，并指定爬取域的范围：
scrapy genspider itcast "itcast.cn"

cd spiders/
vim itcast.py

############################################################
# -*- coding: utf-8 -*-
import scrapy
class ItcastSpider(scrapy.Spider):
    name = 'itcast'
    allowed_domains = ['itcast.cn']
    start_urls = ["http://www.itcast.cn/channel/teacher.shtml",]
    
    def parse(self, response):
        with open("teacher.html","w") as f:
            f.write(response.text)
############################################################
scrapy crawl itcast # 默认teacher.html会下载到执行这个命令时的目录下

vim itcast.py

############################################################
# -*- coding: utf-8 -*-
import scrapy
from mySpider.items import MyspiderItem

class ItcastSpider(scrapy.Spider):
    name = 'itcast'
    allowed_domains = ['itcast.cn']
    start_urls = ["http://www.itcast.cn/channel/teacher.shtml",]

    def parse(self, response):
        items = []

        for each in response.xpath("//div[@class='li_txt']"):
            # 将我们得到的数据封装到一个 `ItcastItem` 对象
            item = MyspiderItem()
            #extract()方法返回的都是unicode字符串
            name = each.xpath("h3/text()").extract()[0]
            level = each.xpath("h4/text()").extract()[0]
            info = each.xpath("p/text()").extract()[0]

            #xpath返回的是包含一个元素的列表
            item['name'] = name[0]
            item['level'] = level[0]
            item['info'] = info[0]

            items.append(item)
            
            #yield item

        # 直接返回最后数据
        return items
############################################################

############################################################
# return
############################################################

# json格式，默认为Unicode编码
scrapy crawl itcast -o teachers.json

# json lines格式，默认为Unicode编码
scrapy crawl itcast -o teachers.jsonl

# csv 逗号表达式，可用Excel打开
scrapy crawl itcast -o teachers.csv

# xml格式
scrapy crawl itcast -o teachers.xml

############################################################
# yield (将return注释,将yield取消注释)
############################################################



cd ..
vim pipelines.py
############################################################
# -*- coding: utf-8 -*-

import json

class MyspiderPipeline(object):
    def __init__(self):
        self.file = open('teacher.json', 'wb')

    def process_item(self, item, spider):
        content = json.dumps(dict(item), ensure_ascii=False) + "\n"
        self.file.write(content.encode('utf8'))
        return item

    def close_spider(self, spider):
        self.file.close()
############################################################

vim settings.py
############################################################
ITEM_PIPELINES = {
    'mySpider.pipelines.MyspiderPipeline': 300,
}
############################################################
scrapy crawl itcast
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# 查看当前目录是否生成teacher.json
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
```



### 3-2. Scrapy结构
#### 3-2-1. 语法
1. spiders下的写法

    1. response对象的方法
        1. `xpath()`: 传入xpath表达式，返回该表达式所对应的所有节点的selector list列表
        2. `extract()`: 序列化该节点为Unicode字符串并返回list
        3. `css()`: 传入CSS表达式，返回该表达式所对应的所有节点的selector list列表，语法同 BeautifulSoup4
        4. `re()`: 根据传入的正则表达式对数据进行提取，返回Unicode字符串list列表
        5. 常用
        ```
        for each in response.xpath("//div[@class='li_txt']"):
            # ...
            name = each.xpath("h3/text()").extract()[0]
        ```
   
2. pipelines
    1. items对象=>json字符串:在`process_item`里写`j=json.dumps(dict(item),ensure_ascii=False)+'\n'`
        
3. crawlSpider
    1. 创造crawlSpider:`scrapy genspider -t crawl tencent tencent.com`
    2. 类:`class scrapy.spiders.CrawlSpider`
    3. rule
    ```  
    from scrapy.linkextractors import LinkExtractor
    from scrapy.spiders import CrawlSpider, Rule
    #....
    class SunSpider(CrawlSpider):
        rules = (
            Rule(LinkExtractor(allow=r'type=4&page=\d+'),follow=True,process_links='handle_links'), # 默认交给process_item
            Rule(LinkExtractor(allow=r'question/\d+/\d+.shtml'),callback='parse_item',follow=False)
        )
    ```
#### 3-2-2. pipeline
```
import something

class SomethingPipeline(object):
    def __init__(self):    
        # 可选实现，做参数初始化等
        # doing something

    def process_item(self, item, spider):
        # item (Item 对象) – 被爬取的item
        # spider (Spider 对象) – 爬取该item的spider
        # 这个方法必须实现，每个item pipeline组件都需要调用该方法，
        # 这个方法必须返回一个 Item 对象，被丢弃的item将不会被之后的pipeline组件所处理。
        return item

    def open_spider(self, spider):
        # spider (Spider 对象) – 被开启的spider
        # 可选实现，当spider被开启时，这个方法被调用。

    def close_spider(self, spider):
        # spider (Spider 对象) – 被关闭的spider
        # 可选实现，当spider被关闭时，这个方法被调用
```



### 3-3. scrapy shell
`scrapy shell "http://www.itcast.cn/channel/teacher.shtml"`

### 3-4. scrapy redis

1. 为什么分布式做不起来

- redis_key为单数
- lpush xxx:start_urls xxx 不用引号
- redis_key用类:start_urls来命名

分布式爬虫只能出现Filter debug等?

- allowed_domains自己设置 不用动态设置的



### 实现

1. 正常写完scrapy
2. 在settings里面添加去重类,调度器,暂停,redis管道文件,队列等(实现redis存储,redis管道文件优先级低)

```
DUPEFILTER_CLASS = "scrapy_redis.dupefilter.RFPDupeFilter"
SCHEDULER = "scrapy_redis.scheduler.Scheduler"
SCHEDULER_PERSIST = True
SCHEDULER_QUEUE_CLASS = "scrapy_redis.queue.SpiderPriorityQueue"
#SCHEDULER_QUEUE_CLASS = "scrapy_redis.queue.SpiderQueue"
#SCHEDULER_QUEUE_CLASS = "scrapy_redis.queue.SpiderStack"

#REDIS_HOST = '10.189.106.209'
#REDIS_PORT = 6379

ITEM_PIPELINES = {
    'youyuan.pipelines.YouyuanPipeline': 300,
    'scrapy_redis.pipelines.RedisPipeline': 900,
}

```



1. 在spider里面删除start_urls,增加redis_key,变成RedisCrawlSpider类等(分布式)

```
# -*- coding: utf-8 -*-
import scrapy
from scrapy.linkextractors import LinkExtractor
from scrapy.spiders import CrawlSpider, Rule
from youyuan.items import *
from scrapy_redis.spiders import RedisCrawlSpider

class YySpider(RedisCrawlSpider):
    name = 'yy'
    allowed_domains = ['hebnews.cn']
    #start_urls = ['http://www.hebnews.cn/']
    redis_key='yyspider:start_urls'

    rules = (
        Rule(LinkExtractor(allow=r'\d+-\d+/\d+/content_\d+'), callback='parse_item', follow=True),
    )

    #def __init__(self, *args, **kwargs):
    #    # Dynamically define the allowed domains list.
    #    domain = kwargs.pop('domain', '')
    #    self.allowed_domains = filter(None, domain.split(','))
    #    super(YySpider, self).__init__(*args, **kwargs)


    def parse_item(self, response):
        item=YouyuanItem()
        item['content']=self.get_content(response)
        item['title']=self.get_title(response)
        item['url']=response.url
        yield item


    def get_content(self,response):
        return "".join(response.xpath('//div[@class="text"]/p/text()').extract())

    def get_title(self,response):
        return response.xpath('//h1/text()').extract()[0]

```



1. 启动redis服务(#bind 127.0.0.1,不用守护进程,不用保护模式)
2. 输入指令

```
lpush youyuan:start_urls http://www.youyuan.com/find/beijing/mm18-25/advance-0-0-0-0-0-0-0/p1/

```





### process_item_for_mysql

```r.blpop('yy:items')```pop出来的结果是tuple

```
(b'yy:items', b'{"content": "5\\u670813\\u65e5\", "title": "\\u738b\\u4e1c\\u5cf0\\uff1a\\u6df1\\u5165\\u624e\\u5b9e\\u5f00\\u5c55\\u653f\\u6cbb\\u6027\\u8b66\\u793a\\u6559\\u80b2\\u63a8\\u52a8\\u5168\\u9762\\u4ece\\u4e25\\u6cbb\\u515a\\u5411\\u7eb5\\u6df1\\u53d1\\u5c55", "url": "http://hebei.hebnews.cn/2018-05/13/content_6879325.htm"}')
```





### 存入数据库
1. 存入MongoDB
    1. 启动MongoDB数据库：`sudo mongod`
    2. 执行下面程序：`py2 process_youyuan_mongodb.py`

```python
# process_youyuan_mongodb.py

# -*- coding: utf-8 -*-

import json
import redis
import pymongo

def main():

    # 指定Redis数据库信息
    rediscli = redis.StrictRedis(host='192.168.199.108', port=6379, db=0)
    # 指定MongoDB数据库信息
    mongocli = pymongo.MongoClient(host='localhost', port=27017)

    # 创建数据库名
    db = mongocli['youyuan']
    # 创建表名
    sheet = db['beijing_18_25']

    while True:
        # FIFO模式为 blpop，LIFO模式为 brpop，获取键值
        source, data = rediscli.blpop(["youyuan:items"])

        item = json.loads(data)
        sheet.insert(item)

        try:
            print u"Processing: %(name)s <%(link)s>" % item
        except KeyError:
            print u"Error procesing: %r" % item

if __name__ == '__main__':
    main()
```

```python
#coding=utf8
import pymongo
import redis
import json

r=redis.Redis(host='localhost',port=6379,db=0)
m = pymongo.MongoClient('mongodb://admin:123@localhost:27017/')
db=m['orris']
collection=db['yy']
item=json.loads(r.lpop('yy:items'))
collection.insert(item)
```



2. 存入 MySQL

    1. 启动mysql：`mysql.server start`（更平台不一样）
    2. 登录到root用户：`mysql -uroot -p`
    3. 创建数据库`youyuan`:`create database youyuan;`
    4. 切换到指定数据库：`use youyuan`
    5. 创建表`beijing_18_25`以及所有字段的列名和数据类型。
    6. 执行下面程序：`py2 process_youyuan_mysql.py`

```
#process_youyuan_mysql.py

# -*- coding: utf-8 -*-

import json
import redis
import MySQLdb

def main():
    # 指定redis数据库信息
    rediscli = redis.StrictRedis(host='192.168.199.108', port = 6379, db = 0)
    # 指定mysql数据库
    mysqlcli = MySQLdb.connect(host='127.0.0.1', user='power', passwd='xxxxxxx', db = 'youyuan', port=3306, use_unicode=True)

    while True:
        # FIFO模式为 blpop，LIFO模式为 brpop，获取键值
        source, data = rediscli.blpop(["youyuan:items"])
        item = json.loads(data)

        try:
            # 使用cursor()方法获取操作游标
            cur = mysqlcli.cursor()
            # 使用execute方法执行SQL INSERT语句
            cur.execute("INSERT INTO beijing_18_25 (username, crawled, age, spider, header_url, source, pic_urls, monologue, source_url) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s )", [item['username'], item['crawled'], item['age'], item['spider'], item['header_url'], item['source'], item['pic_urls'], item['monologue'], item['source_url']])
            # 提交sql事务
            mysqlcli.commit()
            #关闭本次操作
            cur.close()
            print "inserted %s" % item['source_url']
        except MySQLdb.Error,e:
            print "Mysql Error %d: %s" % (e.args[0], e.args[1])

if __name__ == '__main__':
    main()
```

```python
#coding=utf-8
import json
import pymysql
import redis

conn=pymysql.connect(host='127.0.0.1',db='youyuan',port=3306,user='root',password='serena2ash',charset='utf8')
r=redis.Redis(host='localhost',port=6379,db=0)
with conn.cursor() as cursor:
    sql='insert into data(title,content,url) values(%s,%s,%s)'
    item=r.lpop('yy:items')
    d=json.loads(item)

    cursor.execute(sql,[d['title'],d['content'],d['url']])

conn.commit()
```

**charset='utf8'一定要写!!**

