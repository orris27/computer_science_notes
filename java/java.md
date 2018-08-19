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
