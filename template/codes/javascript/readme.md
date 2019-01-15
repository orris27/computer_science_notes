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


+ storage
    1. save
    ```
    sessionStorage.setItem("account", address)
    ```
    2. load
    ```
    const currentAccount = sessionStorage.getItem("account")
    ```



+ timeout
```
    setTimeout(() => {
      const info = document.getElementById('info')
      info.hidden = true
      console.log('ok')
    }, 1.5 * 1000)

```


+ url
```
window.location.href = "index.html"
```

+ refresh
```
location.reload()
```

+ style
```
<a href="student_login.html" class="btn btn-link btn-lg" style="color: #666;" role="button" aria-pressed="true">登录</a>
```

+ 使用外界的包
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
