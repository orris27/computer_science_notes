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



vue事件处理器

1. 写在标签内,作为属性

2. `v-on:click="greet"`或者`@click="greet"`. 其中greet为函数名

`v-on:click="greet"` 

v-on:click.stop, v-on:click.stop.prevent阻止默认事件, v-on-click:self绑定事件对象本身,子元素没用 v-on:click.once只生效一次

v-on:keyup.enter .tab .delete .esc .space .space .up .down .left .right封装好


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
    
#### index.js来引用
(以`src/router/index.js`为例)
1. 静态路由

    1. 写好代表组件的vue文件
    2. 导入: 后缀名可以省略(参考配置文件)
    ```
    import GoodsList from './../views/GoodsList.vue'
    ```
    3. 使用: 给指定uri渲染某个组件
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
    3. 使用并指定变量: 给指定uri渲染某个组件
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
    3. 使用并指定路由: 给指定uri渲染某个组件
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
    






## 4. vue-router

router-link 跳转

router-view 渲染

动态路由匹配

嵌套路由

编程式路由


@ resolve到src中

views 老师创建的目录 放置页面 也是vue文件?

components 放置复用的组件


router的index.js中


路径: main.js 调用 App.vue, App.vue呈现页面,但页面中会有router/index.js的内容,而index.js内容则是根据uri渲染对应的组件

总之,App.vue定义了框架, 而index.js渲染的组件被放置在其中. 所以最终画面 = App.vue + index.js

## 5. 开源项目
### CoreUI
```
git clone https://github.com/coreui/coreui-free-vue-admin-template.git CoreUI-Vue

# 解决chromedriver安装不了的问题
mkdir ~/.npm-global
npm config set prefix '~/.npm-global'
export PATH=~/.npm-global/bin:$PATH
NPM_CONFIG_PREFIX=~/.npm-global

npm install chromedriver
npm install

# run
npm run serve
```
