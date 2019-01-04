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
