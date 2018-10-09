    
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
3. IO
    1. 输入
        1. int
        ```
        import java.util.Scanner;
        
        Scanner input = new Scanner(System.in);
        System.out.println("Enter an integer:");
        int v1 = input.nextInt();
        ```
        2. line
        ```
        import java.util.Scanner;
        
        Scanner input = new Scanner(System.in);
        String line = input.nextLine(); // 貌似要弄2次
        line = input.nextLine();
        ```
4. 数组
    1. 定义一个数组的引用变量: 实际没有指向有效内存
    ```
    int[] intArray;
    int intArray[]; // 建议使用第一种方式,因为第一种方式更体现这个变量是引用变量
    ```
    2. 创建一维数组
    ```
    int[] intArray = new int[5];
    ```
    3. 使用一维数组
    ```
    for(int i=0;i<5;++i)
    {
        arr[i] = i;
    }

    for(int i=0;i<5;++i)
    {
        System.out.println(arr[i]);
    }
    ```
5. global variables
```
public static final int MAX_LEFT = 100; # 定义常量

public static int[] arr = new int[5];
public static void main(String[] args)
{
    // ...
}
```
6. 随机
    1. 产生`[0, 1)`的实数
    ```
    import java.util.Random;
    
    System.out.print(Math.random());
    ```
    2. 产生`a, a+1, a+2 ,..., b-1`的整数,即`[a, b)`: 一定要在用小括号包住Math.random!!!!否则就是0了
    ```
    import java.util.Random;
    
    int a = 3;
    int b = 5;
    System.out.print((int)(Math.random() * (b - a) + a ));
    ```
## 2. System
1. calculate the time
```
long startTime = System.currentTimeMillis();// 取得计算开始时的时间

@SuppressWarnings("unused")
int sum = 0;// 声明变量并初始化
for (int i = 0; i < 1000000000; i++) { // 进行累加
    sum+=i;
}
long endTime = System.currentTimeMillis();// 取得计算结束时间

// 计算所花费的时间
System.out.println("累加所花费的时间" + (endTime - startTime) + "毫秒");
```
2. various ways of printing
    1. println
    ```
    System.out.println("HelloWorld!");
    System.out.println();
    ```
    2. print: without newline
    ```
    System.out.print("no newline");
    ```

## 3. Math
1. min
```
Math.min(3, 5)

double min(double arg1, double arg2)
float min(float arg1, float arg2)
int min(int arg1, int arg2)
long min(long arg1, long arg2)
```
2. pow
```
System.out.println(Math.pow(2,3));
#----------------------------------------------
# 8.0
#----------------------------------------------
```
## 4. String
1. trim: <=>python.strip()
```
String a = "    app le  ";
System.out.println(a.trim());
#----------------------------------------------
# app le
#----------------------------------------------
```
2. replace
```
String a = "  app le  ";
System.out.println(a.replace(" ",""));
#----------------------------------------------
# apple
#----------------------------------------------

```
3. equals: 字符串相等
```
System.out.println("apple".equals("apple"));
#----------------------------------------------
# true
#----------------------------------------------
```
4. length
```
String a = "apple";
System.out.println(a.length());
#----------------------------------------------
# 5
#----------------------------------------------
```
5. charAt: ~~s[i]~~
```
String a = "apple";
for (int i = 0; i < a.length(); ++i)
    System.out.println(a.charAt(i));
#----------------------------------------------
# a
# p
# p
# l
# e
#----------------------------------------------
```
