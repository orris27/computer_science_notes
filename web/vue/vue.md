## 1. 基本使用
### Vue多页面应用文件引用
#### 1. 官网拷贝: `<script src="https://unpkg.com/vue/dist/vue.js"></script>`.
放到head标签下就能直接使用vue的特性了,而且单个html就能使用.当然可以使用国内镜像
#### 2. npm安装
搭建
```
cnpm i vue --save
```
引用这些包: vue.js和vue.min.js都可以,放到head里面就可以了
```
<script src="node_modules/vue/dist/vue.min.js"></script>
```

### vue-cli构建SPA应用
```
npm install -g vue-cli # 安装一次就可以了,以后其他项目就不需要
vue init webpack-simple demo # 初始化一个简单的webpack项目,名字叫做demo.创建一个名字叫demo的目录,项目名为我们自己输入(必须小写)
#或者
vue init webpack demo # 初始化一个完整的webpack项目
```
安装依赖: 根据`package.json`里面的结果运算
```
cnpm install
```
运行: 根据`package.json`里面的`scripts`执行脚本. 如果是完整版的话,也可以`node build/dev-server.js`来启动(就是scripts里的内容)
```
cnpm run dev
```
#### 构建完整版
```
vue init webpack demo2


? Project name demo2
? Project description description
? Author orris
? Vue build standalone
? Install vue-router? Yes
? Use ESLint to lint your code? No
? Set up unit tests No
? Setup e2e tests with Nightwatch? No
? Should we run `npm install` for you after the project has been created? (recommended) npm

cd demo2
npm run dev
```
#### 配置文件
目录介绍
```
build打包的配置文件所在目录

webpack.base.conf.js
build.js: 构建生产版本.开发完后构建生产版本


config打包的配置

index.js:主目录,端口, dev和build的参数

src开发项目的源码
App.vue入口组件
main.js项目的入口文件
static静态资源
.babelrc ES6解析的配置
.editorconfig sublime的配置
index.html单页面应用程序的入口
package.json关于项目的描述,依赖,版本号
```

打包生产包,生成的就是dist目录
```
cnpm run build 
```

1. webpack.base.conf.js
`[]`变量,`[name]`和app一样

reslove下如果extensions里指定类型,那么我们可以不写这些

vue.common.js 针对不同规范的版本

babel-loader 解析ES6

图片如果比较小用base64编码

node_modules项目安装的依赖



src/App.vue会调用src/components/HelloWorld.vue文件


## 2. syntax
Vue对象, el表示元素, data定义变量值
```
<div id="app">
    <span>{{message}}</span>
</div>
<script type="text/javascript">
    new Vue({
        el:"#app",
        data:{
            message:"hello,vue"
            }
    })
</script>
```

Mustache {{msg}} data

html赋值 v-html 当做标签
v-bind 给标签添加属性
表达式{{ok ? "yes":"no"}}
v-text 文本赋值
指令v-if

过滤器 {{message | capitalize}} 和 v-bind:id ="rawId | formatId"

class和style绑定
v-bind:class="{ active:isActive, "text-danger":hasError }"

数组语法
```
<div v-bind:class="[activeClass, errorClass]">
data: {
    activeClass: "active",
    errorClass: "text-danger",
}
```


style绑定-对象语法: v-bind:style = "{color: activeColor, fontSize:fontSize + 'px'}"

条件渲染

v-if

v-else

v-else-if

v-show 显示和隐藏

v-cloak 页面刷新太快,隐藏代码?



vue组件

全局组件和局部组件

父子组件通讯-数据传递 儿子emit到父亲(不允许), 父亲property到儿子.

父亲: 调用组件的vue

儿子: 组件本身

slot


变量

`<template>`标签内使用`{{ var_name }}`

`<script>`标签内使用`this.var_name`



## 3. 组件
### 定义
1. 新建1个vue文件
2. 设置template和script标签
3. 填写html内容:在template内
```
<template>
    <div>
        <button @click='increment'>+</button>
        <button v-on:click= 'decrement'>-</button>
        <p><span>{{num}}</span></p>
    </div>
</template>
```
4. 填写html属性: 在script标签内
    1. data: 定义template内的变量的初始值
    2. method: 定义函数
```
<script>
    export default{
        data(){
            return {
                num: 0,
                msg:'hello vue',
            }
        },
        methods:{
            increment(){
                this.num ++;
            },
            decrement(){
                this.num --;
            },
        }
    }
</script>

```

### 引用
#### vue来引用
1. 写好代表组件的vue文件
2. 导入: 后缀名可以省略(参考配置文件)
```
<script>
    import Counter from './Counter.vue';
    export default {
        // ...
        components:{
            Counter
        }
    }
</script>
```
3. 使用
```
<template>
  <div class="hello">
    <Counter></Counter>
  </div>
</template>
```
4. 传递数据给组件: 父亲给儿子的某个变量赋予值
    1. 给定值: 变量名num必须相同
        1. 父亲给组件一个属性
        ```
        <Counter num="10"></Counter>
        ```
        2. 儿子指明这个属性来自父亲: 写在`export default`里面
        ```
        props:["num"],
        ```
    2. 父亲的变量: 单向绑定
        1. 父亲定义变量
        ```
        <p><span>{{num}}</span></p>
        ```
        2. 父亲初始化变量
        ```
        data () {
        return {
            num: 100,
        // ...
        ```
        3. 父亲给组件一个绑定了该变量的属性
        ```
        <Counter v-bind:num="num"></Counter>
        ```
        4. 儿子指明这个属性来自父亲: 同4-1

    3. 父亲的变量: 双向绑定
        1. 父亲定义变量: 同4-1
        2. 父亲初始化变量: 同4-1
        3. 父亲给组件一个绑定了该变量的属性: 同4-1
        4. 父亲给儿子提供一个反馈的渠道: `v-on:儿子的反馈渠道="我的处理方法"`
        ```
        <Counter v-bind:num="num" v-on:incre="increment" v-on:decre="decrement"></Counter>
        ```
        5. 父亲定义收到反馈的处理
        ```
        methods:{
            increment(){
                this.num ++;
            },
            decrement(){
                this.num --;
            },
        },
        ```
        6. 儿子指明这个属性来自父亲: 同4-1
        7. 儿子定义发送反馈方法
        ```
        methods:{
            increment(){
                //this.num ++;
                this.$emit("incre");

            },
            decrement(){
                this.$emit("decre");
                //this.num --;
            },
        }
        ```
    
#### index.js来引用(路由)
`this.$router.options`表示的是index.js里的所有内容

(以`src/router/index.js`为例)
1. 静态路由
    1. 写好代表组件的vue文件
    2. 导入: 后缀名可以省略(参考配置文件)
    ```
    import GoodsList from './../views/GoodsList.vue'
    ```
    3. 分配路由: 给指定uri渲染某个组件
    ```
    routes: [
        {
            path: '/',
            name: "GoodsList",
            component: GoodsList
        }
    ]    
    ```
2. 动态路由: hash(url中会有`#`)
    1. 写好代表组件的vue文件
    2. 导入: 同1-1
    3. 分配路由并指定变量: 给指定uri渲染某个组件
    ```
    path: '/goods/:goodsId/other/:goodsId2',
    ```
    4. 组件中使用路由的变量
    ```
    <h1>GoodsList: {{$route.params.goodsId}} + {{$route.params.goodsId2}}</h1>
    ```

3. 动态路由: 非hash(url中删除`#`)
```
export default new Router({
    mode: 'history',
    //...
)}
```

4. 嵌套路由: 在既有路由下页面的部分区域分配子路由
    1. 写好代表组件的vue文件
    2. 导入: 同1-1
    3. 分配路由: 给指定uri渲染某个组件
    ```
    routes: [
        {
            path: '/goods',
            name: 'GoodsList',
            component: GoodsList,
            children:[
                {
                    path: 'title',
                    name: 'title',
                    component: Title,
                },
                {
                    path: 'image',
                    name: 'image',
                    component: Image,
                },
            ]
        }
    ]
    ```
    4. 定义子路由展示位置: router-view
    ```
    <template>
        <div>
            <h1>GoodsList: {{$route.params.goodsId}} + {{$route.params.goodsId2}}</h1>
            <router-link to="/goods/title">go to title</router-link>
            <router-link to="/goods/image">go to image</router-link>
            <div>
                <router-view></router-view>
            </div>
        </div>
    </template>
    ```
    
    5. 定义子路由跳转的超链接: 注意必须绝对路由 && router-link
    ```
    <template>
        <div>
            <h1>GoodsList: {{$route.params.goodsId}} + {{$route.params.goodsId2}}</h1>
            <router-link to="/goods/title">go to title</router-link>
            <router-link to="/goods/image">go to image</router-link>
            <div>
                <router-view></router-view>
            </div>
        </div>
    </template>
    ```
    
5. 编程式路由: 利用html的事件触发js代码,然后在js代码里跳转(原来:html的标签跳转)
    1. 写好button
    ```
    <button v-on:click="jump">button_go_to_new_page</button>
    ```
    2. 绑定事件
    ```
    export default{
        methods:{
            jump(){
                this.$router.push("/goods/image");
                //this.$router.push("/goods/image")
                //this.$router.push({path: "/goods/image"})
                //this.$router.push({path: "/goods/image?goodsId=123"})
                //this.$router.go(-2)
            },
        }
    }
    ```
    3. (被渲染的组件使用路由里面的数据): 提取路由的参数2种方法: 1. 从动态路由(params) 2. 从编程式路由(query)
    ```
    <h1>GoodsList: {{$route.query.goodsId}}</h1>

    ```

6. 命名路由: 利用路由的名字跳转
    1. 不设置参数
    + v-bind不能省略
    + 'image'是定义在index.js里的路由name,可以chidren里面的,必须有单引号
    ```
    <router-link v-bind:to="{name: 'image'}">go to image</router-link>
    ```
    2. 设置参数
    ```
    <router-link v-bind:to="{name: 'image',params:{imageId: 123}}">go to image</router-link>
    
    // ....
    
    path: '/goods/image/:imageId',
    name: 'image',
    ```

## 4. vue-router










router-link 跳转

router-view 渲染

动态路由匹配

嵌套路由

编程式路由


@ resolve到src中

views 老师创建的目录 放置页面,用来看的 也是vue文件

components 放置复用的组件


router的index.js中


路径: main.js 调用 App.vue, App.vue呈现页面,但页面中会有router/index.js的内容,而index.js内容则是根据uri渲染对应的组件

总之,App.vue定义了框架, 而index.js渲染的组件被放置在其中. 所以最终画面 = App.vue + index.js



## 4. vue-resource(不再维护,推荐使用axios)
### 导入
#### html文件中使用(html文件可以放到static下面,直接/xx.html是索引不到的,但是/static/xx.html可以)
本质上都是用script引入.需要先引入vue.js
1. 网络资源
2. 本地资源:npm + node_modules: `cnpm i vue-resource --save`(安装项目依赖)
```
<script src="node_modules/vue-resource/dist/vue.js"></script>
<script src="node_modules/vue-resource/dist/vue-resource.js"></script>
```
#### vue文件中使用
在`main.js`或者`.vue`内的script中写入
```
import VueResource from 'vue-resource'
Vue.use(VueResource) // 如果在main.js中写入的话
```
### 请求
1. html中定义某个元素,这个元素触发事件,这个事件执行get,post等
```
# 例子
<a href="javascript:;" v-on:click="get">get</a>
```
2. 定义get / post等函数
+ this.msg是变量,单纯用来显示
+ error的函数是在请求失败的时候调用的,error为错误时返回的json数据
+ res.data才是json格式的数,当然需要后续继续操作
+ `this.$http`<=>`Vue.http`
```
get(){
    this.$http.get("http://localhost:8080/static/package.json",{
        params:{
            userId: "101",
        },
        headers: {
            tokens: "abcd",
        }

    }).then(res => {
        this.msg = res.data;
        console.log("ok");

    }, error => {
        this.msg = error;
    })
},


post(){
    this.$http.post("http://localhost:8080/static/package.json",{
        userId: "101",
    },{
        headers: {
            tokens: "abcd",
        }
    }).then(res => {
        this.msg = res.data;
        console.log("ok");

    }, error => {
        this.msg = error;
    })
},
```

### http
将请求的公共URL前缀提取出来,从而方便写
```
http:{
    root: "http://localhost:8080/static/",

},
methods:{
    get(){
        this.$http.get("package.json",{ // 这里就不用写http://localhost:8080/static/package.json了
        //...

```


## 6. axios
暴露了axios全局变量,并没有被装入到this里面
### 导入
1. 网络
2. 本地: npm + node_module: `npm install axios --save`
#### html中使用
```
<script src="../node_modules/axios/dist/axios.js"></script>
```
#### vue文件中使用
在`main.js`或者`.vue`内的script中写入
```
import axios from 'axios'
```
### 请求
```
get(){
    axios.get("",{
        params:{
            userId: "101",
        },
        headers: {
            token: "orris",
        },
    }).then(res => {
        this.msg = res.data;
    }).catch(error => {
        console.log("err init." + error)
    })
}



post(){
    axios.post("http://localhost:8080/static/package.json",{
        userId: "101",
    },{
        headers: {
            tokens: "abcd",
        }
    }).then(res => {
        this.msg = res.data;
        console.log("ok");

    }).catch(error => {
        console.log("err init." + error)
    })
},

// <a href="javascript:;" v-on:click="http">http</a>
http(){
    axios({
        url: "http://localhost:8080/static/package.json",
        method: "get",
        //data:{ //使用在post里面
        //    userId: "101",    
        //},
        params: { // 使用在get里面
            userId: "101",
        },
        headers:{
            token: "http-test"
        },
        
    }).then (res => {
        this.msg = res.data;
    }).catch (error => {
        console.log(error);
    })

}
```

### 拦截器
在接收所有的请求之前执行这段代码. 直接放在export default下面
```
export default {
    mounted() {
        axios.interceptors.request.use(config => {
            console.log("request init");
            return config;
        })
    },
}
```
在返回所有的response之前执行这段代码. 直接放在export default下面
```
export default {
    mounted() {
        axios.interceptors.response.use(response => {
            console.log("response init");
            return response;
        })
    },
}
```

## 7. 开源项目
### CoreUI
安装方法: 我的Ubuntu必须要先执行`npm install chromedriver`,但可以不执行中间`mkdir`那一段
```
git clone https://github.com/coreui/coreui-free-vue-admin-template.git CoreUI-Vue
cd CoreUI-Vue

# 解决chromedriver安装不了的问题
mkdir ~/.npm-global
npm config set prefix '~/.npm-global'
export PATH=~/.npm-global/bin:$PATH
NPM_CONFIG_PREFIX=~/.npm-global

npm install chromedriver # 必须要执行这一步
npm install

# run
npm run serve
```

### vue-element-admin
```
git clone https://github.com/PanJiaChen/vue-element-admin.git
cd vue-element-admin
npm install
npm run dev
```
### vue-admin(不再维护)
```
git clone https://github.com/vue-bulma/vue-admin.git
cd vue-admin
npm install
cd node_modules/bulma/sass/utilities
wget https://raw.githubusercontent.com/jgthms/bulma/master/sass/utilities/initial-variables.sass
cd -
npm run dev
```


## 8. 技术栈
以vuethink为例,下面为技术栈
+ 后端框架：ThinkPHP 5.0.x/ThinkPHP 5.1.x
+ 前端MVVM框架：Vue.JS 2.x
+ 开发工作流：Webpack 1.x
+ 路由：Vue-Router 2.x
+ 数据交互：Axios
+ 代码风格检测：Eslint
+ UI框架：Element-UI 1.1.6
JS函数库：Lodash


## 9. 指令
### 9-1. 条件
1. v-if也可以作用在template标签,其他还有v-else-if和v-else
2. v-if的内容可以是
    1. boolean变量
    2. boolean表达式,如`num === 111`
3. v-if为false的时候,不会保留在DOM中,而`v-show`会,因为`v-show`改变display属性
```
<p v-if="seen">hello</p>

data () {
  retrun {
    seen: true, // false => hide
  }
}
```
#### 复用
下面的代码会复用input内的值
```
<template v-if="loginType === 'username'">
  <label>Username</label>
  <input placeholder="Enter your username">
</template>
<template v-else>
  <label>Email</label>
  <input placeholder="Enter your email address">
</template>
```
下面的代码通过key保证不同input内的值不同
```
<template v-if="loginType === 'username'">
  <label>Username</label>
  <input placeholder="Enter your username">
</template>
<template v-else>
  <label>Email</label>
  <input placeholder="Enter your email address">
</template>
```
#### v-show
```
<p v-show="seen">hello</p>

data () {
  retrun {
    seen: true, // false => hide
  }
}
```
### 9-2. 绑定标签的属性到vue变量中
```
<p v-bind:title="msg">hello</p>

data () {
  retrun {
    msg: "hello", 
  }
}
```
### 9-3. 遍历数组变量v-for
```
<div id="app-4">
  <ol>
    <li v-for="todo in todos">
      {{ todo.text }}
    </li>
  </ol>
</div>


var app4 = new Vue({
  el: '#app-4',
  data: {
    todos: [
      { text: '学习 JavaScript' },
      { text: '学习 Vue' },
      { text: '整个牛项目' }
    ]
  }
})
```
### 9-4. 绑定表单v-model
在表单`<input>`、`<textarea>`及`<select>`元素上创建双向数据绑定.和num变量绑定,无论改变文本框的内容还是变量的值,双向都改动
+ 忽略所有表单元素的`value`,`checked`,`selected`特性的初始值,而只是用变量的初始值 => 因此需要申明变量的初始值
文本框
```
<input v-model="num"/>
```
单选框
```
<input type="checkbox" id="checkbox" v-model="checked">
<label for="checkbox">{{ checked }}</label>
```
单选按钮,选择框参考[官方文档](https://cn.vuejs.org/v2/guide/forms.html)

#### 修饰符
1. `.number`: 自动将用户的输入转换为数值类型
```
<input v-model.number="age" type="number">
```
2. `.trim`: 自动删除用户的输入的首尾空白符号
```
<input v-model.trim="msg">
```
### 9-5. v-on监听事件,并执行js代码
vue事件处理器. 绑定标签的事件和Vue的函数

1. 写在标签内,作为属性
2. `v-on:click="greet"`或者`@click="greet"`或者`v-on:click="count +=1 "`或者`v-on:click="say("hi");"`. 其中greet,say为函数名.count为变量名
  + `$event`是特殊变量,作为参数,可以用来在js代码中提取原始DOM事件,如
  ```
  <button v-on:click="warn('Form cannot be submitted yet.', $event)">
    Submit
  </button>
  
  methods: {
    warn: function (message, event) {
      // 现在我们可以访问原生事件对象
      if (event) event.preventDefault()
      alert(message)
    }
  }
  ```

#### 修饰符
`.`后缀:修饰这个绑定
+ `.stop`
+ `.prevent`
+ `.capture`
+ `.self`
+ `.once`
+ `.passive`
+ 按键修饰符:根据某个按键修饰,具体参考:[官方文档](https://cn.vuejs.org/v2/guide/events.html)
```
<!-- 阻止单击事件继续传播 -->
<a v-on:click.stop="doThis"></a>

<!-- 提交事件不再重载页面 -->
<form v-on:submit.prevent="onSubmit"></form>

<!-- 修饰符可以串联 -->
<a v-on:click.stop.prevent="doThat"></a>

<!-- 只有修饰符 -->
<form v-on:submit.prevent></form>

<!-- 添加事件监听器时使用事件捕获模式 -->
<!-- 即元素自身触发的事件先在此处理，然后才交由内部元素进行处理 -->
<div v-on:click.capture="doThis">...</div>

<!-- 只当在 event.target 是当前元素自身时触发处理函数 -->
<!-- 即事件不是从内部元素触发的 -->
<div v-on:click.self="doThat">...</div>

<!-- 点击事件将只会触发一次 -->
<a v-on:click.once="doThis"></a>
```
v-on:click.stop, v-on:click.stop.prevent阻止默认事件, v-on-click:self绑定事件对象本身,子元素没用 v-on:click.once只生效一次

v-on:keyup.enter .tab .delete .esc .space .space .up .down .left .right封装好
#### 缩写
`@`


### 9-6. v-bind

#### class和style绑定
拼接字符串结果容易出错,Vue在绑定class和style的时候做了增强

##### 使用
1. 开关
  1. 方法1
  + `<div v-bind:class="{active: isActive}">content</div>`表示这个标签是否具有active这个class取决于isActive是true还是false.如
  ```
  isActive:true, // 如果是false的话就不存在active这个class了

  .active{
    color: #42b983;
  }
  ```
  2. 方法2
  + `<div v-bind:class="{obj1}">content</div>`
  ```
  obj1: {
    active: true,
  }
  ```
#### 缩写
`:`, 如`<p :id="num">:test</p>`就绑定了这个元素的id属性和变量num


## 问题
1. Vue修改了router/index.js里的内容结果刷新没用?
似乎要重新`npm run dev`的样子...


------------------


v-once允许标签内的变量只改变一次,如`<span v-once>{{num}}</span>`

如果我的变量是html语言怎么办?
1. 直接`{{html_words}}`是直接文本
2. 用`<span v-html="html_words"></span>`就会解析html_words里的html语法了

`{{}}`和`v-bind:xxx=""`里的内容都会作为JS被Vue解析,但必须是单一的表达式

`v-bind:标签的属性:"js内容"`: v-bind会将标签的属性和js内容进行绑定.注意`js内容`至少是一个变量


## 10.  Vue实例
所有的Vue组件都是Vue实例

### 10-1. 初始化Vue实例
通过词典完成
1. 属性: data: xxx
2. 方法: `created: function(){}` 或者`created(){}`

Vue的初始化中key为data对应的数据和变量双向绑定

### 10-2. 属性和方法
Vue实例暴露了一些有用的实例属性和方法,都有前缀`$`,区别于用户定义的属性和方法,比如`vm.$data`,`vm.$el`,`vm.$watch`

#### Vue实例常用方法
1. computed: 定义一系列方法,这些方法会修改变量(变量名===方法名)的值. => 允许某些变量是另外一些变量的某种变换.避免在标签内做运算
+ 基于依赖进行缓存.也就是说只要msg这些变量不改变就不会重新运算一遍
+ 也可以用getter和setter方法
```
// var2利用computed属性跟踪var1
<input v-model="var1">
<p>{{var2}}</p>

computed: {
  var2(){
    return this.var1+'_2';
  }
},
```
2. watch: 类似于computed方法,适合于响应某些数据的变化.当需要在数据变化时执行异步或开销较大的操作时，这个方式是最有用的。
```
// var2利用watch属性跟踪var1
<input v-model="var1">
<p>{{var2}}</p>

watch: {
  var1:function() {
    console.log("var1 changed");
    this.var2 = this.var1;
  }
},
```


### 10-3. 生命周期
Vue实例的生命周期: 方法: `created`(实例创建之后执行代码): 
![lifecycle](https://cn.vuejs.org/images/lifecycle.png)

不要在选项属性或回调上使用箭头函数，比如 created: () => console.log(this.a) 或 vm.$watch('a', newValue => this.myMethod())。因为箭头函数是和父级上下文绑定在一起的，this 不会是如你所预期的 Vue 实例，经常导致 Uncaught TypeError: Cannot read property of undefined 或 Uncaught TypeError: this.myMethod is not a function 之类的错误。


### 10-4. Vue组件
可复用的Vue实例就是Vue组件
#### Vue组件特殊于普通Vue实例的点
1. data必须是一个函数: 防止通过变量影响到其他组件
```
data: function () {
  return {
    count: 0
  }
}
```
