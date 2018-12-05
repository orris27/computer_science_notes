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
v-on:click="greet"
v-on:click.stop, v-on:click.stop.prevent阻止默认事件, v-on-click:self绑定事件对象本身,子元素没用 v-on:click.once只生效一次
v-on:keyup.enter .tab .delete .esc .space .space .up .down .left .right封装好

vue组件
全局组件和局部组件
父子组件通讯-数据传递 儿子emit到父亲(不允许), 父亲property到儿子.

slot








## 3. 开源项目
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
