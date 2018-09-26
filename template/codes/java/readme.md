    
## 1. Java
1. HelloWorld
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
2. 进制转换
```
十进制转成十六进制： 
Integer.toHexString(int i) 

十进制转成八进制:
Integer.toOctalString(int i) 

十进制转成二进制:
Integer.toBinaryString(int i) 

十六进制转成十进制:
Integer.valueOf("FFFF",16).toString() 

八进制转成十进制 :
Integer.valueOf("876",8).toString() 

二进制转十进制:
Integer.valueOf("0101",2).toString() 



有什么方法可以直接将2,8,16进制直接转换为10进制的吗? 
java.lang.Integer类 
parseInt(String s, int radix) 
使用第二个参数指定的基数，将字符串参数解析为有符号的整数。 
examples from jdk: 
parseInt("0", 10) returns 0 

parseInt("473", 10) returns 473 

parseInt("-0", 10) returns 0 

parseInt("-FF", 16) returns -255 

parseInt("1100110", 2) returns 102 

parseInt("2147483647", 10) returns 2147483647 

parseInt("-2147483648", 10) returns -2147483648 

parseInt("2147483648", 10) throws a NumberFormatException 

parseInt("99",throws a NumberFormatException 

parseInt("Kona", 10) throws a NumberFormatException 

parseInt("Kona", 27) returns 411787 



进制转换如何写（二，八，十六）不用算法 

Integer.toBinaryString 

Integer.toOctalString 

Integer.toHexString 
```
3. 输入
```
Scanner input1 = new Scanner(System.in);
System.out.println("Enter an integer:");
int v1 = input1.nextInt();
```
