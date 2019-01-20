## 1. Linux下Java开发
### 1-1. 基本使用
1. 安装jdk.可以参考Hadoop里面的[安装文档](https://github.com/orris27/orris/blob/master/bigdata/hadoop/hadoop.md)
2. 创建java文件
3. 使用javac生成java的字节码
4. 使用java运行Java程序
```
java -version

cat >HelloWorld.java <<EOF
public class HelloWorld 
{
    public static void main(String[] args)
    {
        System.out.println("Hello,World!");
        return;
    }
}
EOF

javac HelloWorld.java 
java HelloWorld
```
### 1-2. Hadoop使用
指定classpath就可以了.
+ classpath要精确到jar文件,而不是简单的目录名
> 参考[Hadoop文档](https://github.com/orris27/orris/blob/master/bigdata/hadoop/hadoop.md)

### 1-3. 命令行工具
#### 1-3-1. javac
将java文件转换成JVM能识别的字节码.注意:gcc是直接将C文件转化成机器能识别的机器码,但javac转换得到的字节码还需要JVM去识别并根据对应的操作系统执行.
1. 参数
    1. `-classpaht/-cp`:覆盖CLASSPATH,使用指定引用的外部jar/class文件.Linux下用`:`隔开
    2. `-d`:指定输出的class文件存放的目录.
#### 1-3-2. java
#### 1-3-3. jar
用法和tar非常类似
1. 参数
    1. `cvf`:打包class文件成1个jar文件
    2. `xvf`:解压jar文件=>class文件+`META-INF/MANIFEST.MF`
    3. `-C`:更改为指定的目录并包含其中的文件
    4. `-m`:包含指定清单文件中的清单信息
#### 1-3-4. javap
1. 没有参数:输出类里的所有方法和成员
2. 参数
    1. `-c`:输出字节码
将javac编译的class文件反汇编
```
javac Demo.java
javap Demo
javap -c Demo # 输出字节码
```



## 2. Eclipse
### Ubuntu下安装Eclipse
```
tar zxf eclipse-jee-photon-R-linux-gtk-x86_64.tar.gz
mv eclipse /usr/local/
ln -s /usr/local/eclipse/eclipse /usr/local/bin/eclipse
sudo eclipse

关闭welcome
可以修改Appearacne为classic等
Open Perspective(右上角的小按钮)>Java
大纲方左边,选项卡组方西面
显示Java的控制台Console
```

### 新建Java项目
```
右键>New>Java Project>项目名字写hadoop,JRE会自动检测,默认就好>完成
```
### demo
```
右键>New>类>Package:com.orris.hadoop+Name:TestIDE+勾选public static void main(String[] args)>完成
System.out.println("Hello world");
右键,点击run as>Java Application,如果下方Console出现Hello World,就说明安装好了
```
### 自动补全
```
alt+/ 自动补全(Ctrl+Space显示候选内容)
    window>keys>contenct Assist>Unbind Commnad>Binding:Alt+/>Ok(有冲突,所以删除冲突的)
```
### 安装Hadoop插件
```
安装插件
    1. 下载hadoop2x-eclipse-plugin.zip
    2. 将zip里的release下的*.jar拷贝eclipse的plugins目录下
    3. 重启eclipse就行了
    4. window>Hadoop这个条目如果存在的话,就说明安装好了
New Perspective>MapReduce>New Location>location name:localhost(选择namenode的ip地址,不过好像是随便取的),dfs master选择namenode的ip:8020
```


### 安装Vim插件Vrapper
1. help > Install New Software
2. Work with:  `http://vrapper.sourceforge.net/update-site/stable`,回车
3. 勾选所有, 一路Next,Finish. 然后过一段时间会说没有什么凭证之类的,是否继续安装,就继续安装并重启



### 安装subclipse
`https://dl.bintray.com/subclipse/releases/subclipse/latest/`,其他同上

### 视图




## 3. Syntax
### 3-1. 注释
1. 单行注释:`//`
2. 多行注释:`/**/`
3. 文档注释:被javadoc工具解析生成一个说明书
### 3-2. 常量
1. 字面值常量
    1. 字符串常量:双引号括起来的就是字符串常量
    2. 整数常量
        1. 进制
            1. 二进制:0b开头
            2. 八进制:0开头
            3. 十进制:默认
            4. 十六进制:0x开头
    3. 小数常量
    4. 字符常量:单引号括起来的1个字符就是字符常量
    5. Boolean常量
        1. true/false
        2. 可以输出
    6. 空常量
        1. null
        2. 不可以输出
    7. 自定义常量:`final int SIZE = 3`
2. 自定义常量
### 3-3. 变量
#### 3-3-1. 数据类型
1. 类型
    1. 基本数据类型
        1. 数值型
            1. 整数类型
                1. byte:1个字节(后缀b)
                2. short:2个字节(后缀s)
                3. int:4个字节(整数默认)
                4. long:8个字节(后缀L)
            2. 浮点类型
                1. float:4个字节
                2. double:8个字节(浮点数默认)
        2. 字符类型
            1. char:2个字节
            2. utf8编码的吗
        3. 布尔类型
            1. boolean:1个字节
    2. 引用数据类型
        1. class
        2. interface
        3. []

2. 转换
    1. 默认转换
        1. 加法
            1. 变量
                1. 字符类型转换成整数类型(注意:最后打印是整数类型,但赋值给char变量的话,最后表现形式还是字符)
                2. 字符串类型不变,转为拼接
                3. 变量加法提升类型.如:2个byte变量做加法也是都提升到int类型
            2. 常量:先运算再看类型.如:2个整数常量做加法,最终结果根据赋值的对象的类型等而
        2. 运算中默认将小的类型提升到大的类型.比如"byte的变量x+int的变量y"会将x转换成int类型.最终为int类型
        3. 损失精度的转换默认不被允许,比如给byte类型变量赋值int值
    2. 强制类型转换
        1. 沿用C,但类型加括号而不是内容:`int a=10;byte b=(byte)a;`
        2. 常量有默认类型,也要考虑在内!!如"int a=10;byte b=(byte)a-1"是错误的
### 3-4. 是否沿用
#### 3-4-1. C/C++
1. 沿用
    1. 自增:`++`
    2. static
        1. 静态方法不能调用非静态方法/成员
    3. 强制类型转换
2. 不沿用
    1. 指针
    2. `&`的引用
#### 3-4-2. Python
1. 沿用
2. 不沿用
    1. 字符类型的乘法:`"-"*30`不能在Java中使用


## 4. mvn
### 安装
```
wget http://mirrors.sonic.net/apache/maven/maven-3/3.6.0/binaries/apache-maven-3.6.0-bin.tar.gz
tar zxf apache-maven-3.6.0-bin.tar.gz
echo 'export MAVEN_HOME=/home/orris/tools/apache-maven-3.6.0' >> ~/.bashrc
echo 'export PATH=$MAVEN_HOME/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
mvn --version
#--------------------------------------------------------------------------------------------
# Apache Maven 3.6.0 (97c98ec64a1fdfee7767ce5ffb20918da4f719f3; 2018-10-25T02:41:47+08:00)
# Maven home: /home/orris/tools/apache-maven-3.6.0
# Java version: 1.8.0_181, vendor: Oracle Corporation, runtime: /usr/local/jdk1.8.0_181/jre
# Default locale: en_US, platform encoding: UTF-8
# OS name: "linux", version: "4.13.0-46-generic", arch: "amd64", family: "unix"
#--------------------------------------------------------------------------------------------

```
### 创建项目
```
mvn archetype:generate -DgroupId=com.orris -DartifactId=Crawler -DarchetypeArtifactId=maven-archetype-quickstart -DinteractiveMode=false

orris@orris-Laptop:~/fun/mvn$ tree Crawler/
Crawler/
├── pom.xml
└── src
    ├── main
    │   └── java
    │       └── com
    │           └── orris
    │               └── App.java
    └── test
        └── java
            └── com
                └── orris
                    └── AppTest.java

9 directories, 3 files



```
### 配置
`pom.xml`: 管理整个项目,Maven必须有pom.xml
## 5. IntellJ
### 安装
```
# 去官网下载ideaIC-2018.3.1.tar.gz

tar zxvf ideaIC-2018.3.1.tar.gz

cd idea-IC-183.4588.61/
sudo chmod a=+rx bin/idea.sh 
sudo bin/idea.sh
#############################################
# 安装的时候所有东西都勾选上吧
#############################################
```
### 启动
```
cd ~/tools/idea-IC-183.4588.61/
sudo bin/idea.sh
```
### 创建项目
1. IntelliJ自带的方法
我选择的是Hello World
2. mvn方法(我目前使用的)
```
mvn archetype:generate -DgroupId=com.orris -DartifactId=Crawler -DarchetypeArtifactId=maven-archetype-quickstart -DinteractiveMode=false
```
然后在IntelliJ中点击Open,选择对应的pom.xml就可以了
### 字体
File > Settings > Editor > Font > 24

File > Settings > Editor > Font > Noto Sans Mono CJK TC Bold

### MVN项目
#### pom.xml
##### 1. 利用pom.xml导入crawler4j
1. 查看External Libraries里面有没有我们的crawler4j
2. 如果没有的话,先编辑pom.xml,加入下面代码(谷歌搜索`maven crawler4j`就能找到)
```
    <dependency>
        <groupId>edu.uci.ics</groupId>
        <artifactId>crawler4j</artifactId>
        <version>4.4.0</version>
    </dependency>
```
3. 然后右键pom.xml > Maven > Reimport, 重新导入依赖
4. 再次查看External Libraries,是不是就有东西出来了~~




## 2. IntelliJ
### 运行iml项目

1. git clone
2. `import project`或者`open from existing sources`项目的目录
3. 一系列next:
+ Create project from existing sources > 不要overwrite,选择reuse
3. 配置好Java版本,具体看下面的部分(类似于javafx这些包没有导入都是因为版本问题,javafx换成1.8就可以用了)
4. Build Project
5. 可以看到Main类旁边有个绿色的箭头,可以run了

### 切换版本
1. JAVA_HOME
2. File > Project Structure > Project > Project SDK + Project Language Level都换成对应版本
3. 右上角的Main点击,下拉列表里有Edit Configuration > Application > Main > JRE 切换成对应版本

### run in parallel
Run > Edit Configuration > Allow running in parallel (at the left-top corner)
