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
#### 1-3-2. java
#### 1-3-3. jar
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



## 2. Ubuntu安装Eclipse
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
右键>New>Java Project>项目名字写hadoop,JRE会自动检测,默认就好>完成
右键>New>类>Package:com.orris.hadoop+Name:TestIDE+勾选public static void main(String[] args)>完成
System.out.println("Hello world");
右键,点击run as>Java Application,如果下方Console出现Hello World,就说明安装好了
alt+/ 自动补全(Ctrl+Space显示候选内容)
    window>keys>contenct Assist>Unbind Commnad>Binding:Alt+/>Ok(有冲突,所以删除冲突的)
安装插件
    1. 下载hadoop2x-eclipse-plugin.zip
    2. 将zip里的release下的*.jar拷贝eclipse的plugins目录下
    3. 重启eclipse就行了
    4. window>Hadoop这个条目如果存在的话,就说明安装好了
New Perspective>MapReduce>New Location>location name:localhost(选择namenode的ip地址,不过好像是随便取的),dfs master选择namenode的ip:8020
```

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
