## 1. Vue
https://github.com/orris27/orris/blob/master/web/vue/vue.md


## 2. original
### json
1. json => string
```
JSON.stringify
```
2. string => json
+ 字符串的格式为'{"hello";123}',中间一定是双引号!
```
let b = JSON.parse(a)
```

3. store in the localStorage
+ 为什么我在js中创建好了json对象后,想写到localStorage里后,变成了[object Object]了呢
    > 直接将表示json的变量写入,那么写入的是引用.我们实际上想要的是表示JSON的字符串,所以其实需要先将JSON变量转换为字符串
    > 因为我写了`sessionStorage.setItem('buy_price',prices)`
```
sessionStorage.setItem('buy_price',JSON.stringify(prices))
```

### request
1. fetch
```
fetch("http://127.0.0.1:8080/account/username_pwd.json")
.then(res=>res.json())
.then(
  (res)=>{
    console.log(res)
  }
)
```
2. 跨域请求

前端不用各种headers

[fetch跨域该怎么写？](https://www.zhihu.com/question/47029864)


3. storage
    1. save
    ```
    sessionStorage.setItem("account", address)
    ```
    2. load
    ```
    const currentAccount = sessionStorage.getItem("account")
    ```



4. timeout
```
    setTimeout(() => {
      const info = document.getElementById('info')
      info.hidden = true
      console.log('ok')
    }, 1.5 * 1000)

```


5. url
```
window.location.href = "index.html"
```

6. refresh
```
location.reload()
```

7. style
```
<a href="student_login.html" class="btn btn-link btn-lg" style="color: #666;" role="button" aria-pressed="true">登录</a>
```

8. 使用外界的包
    1. app.js直接import
    ```
    import { sha256, sha224 } from 'js-sha256';

    ```
    2. 其他js用require
    ```
    const utils = require('./utils')
    var sha256 = require('js-sha256');

    module.exports = {
      // 注册客户
      newStudent: function (DinningInstance, account) {
        const address1 = document.getElementById('studentAddress').value
        const address = '0x' + sha256.sha256(address1)
    ```

9. 修改元素内部的HTML
```
document.getElementById('usernameLable').innerHTML = sessionStorage.getItem('username')
```


## 3. Webpack
1. 如何在html中导入css文件?

如果`webpack.conf.js`的entry是`app.js`的话,那么在app.js中写入`import '../stylesheets/bootstrap.css'`,其中的路径是相对于`app.js`而言的

2. 如何在html中导入js文件? (2种方法)
    1. 写个header.js (注意用vim修改的时候,双引号和单引号不要弄错了)
    ```
    module.exports = {

      showHeader: function () {

        document.writeln('<nav class="navbar navbar-expand-lg navbar-light bg-light">');
        document.writeln('  <a class="navbar-brand" href="index.html">导航</a>');
        //...
      },
    }
    ```
    2. 然后在`app.js`中`const header = require('./header')`导入.
    3. 在window.App中添加`showHeader`函数
    ```
    window.App = {
      init: function () {
        //..
      },
      showHeader: function () {
        header.showHeader()
      },
    ```
    4. 在html的header里面调用这个函数
    ```
    <head>
        <title>用户</title>
        <script src="./app.js"></script>
        <script>
          window.App.showHeader()
        </script>
    </head>

    ```


## 4. Bootstrap
1. 如果样式不一样怎么办?

直接修改Bootstrap里的css,比如`btn-outline-secondary`里的`hover`属性我就直接注释掉了
