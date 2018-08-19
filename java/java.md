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
