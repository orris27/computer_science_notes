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

   + 十六进制 => 二进制

   + 十六进制 => 十进制

     ```
     Integer.valueOf("FFFF",16).toString();
     parseInt("-FF", 16); //returns -255
     ```


   + 二进制 => 十进制

     ```
     Integer.valueOf("0101",2).toString();
     parseInt("1100110", 2); //returns 102 
     ```


   + 二进制 => 十六进制

   + 十进制 => 二进制

     ```
     Integer.toBinaryString(int i);
     ```


   + 十进制 => 十六进制

     ```
     Integer.toHexString(int i);
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
7. array
    1. anonymous
    ```
    private static void print(int[] a)
    {
        for (int i = 0;i < a.length; ++i)
        {
            System.out.println(a[i]);
        }
    }
    ```
    2. 锯齿状的二维数组
    ```
    int[][] a = {{1,2},{3,4},{5,6,7}};
    int[][] a = new int[2][3];
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
## 5. Objects and Class
### this
```
this.
```



### new & valueOf
```
int a = new Integer(3);
int b = 3; // => Integer.valueOf(3);
```


### 静态工厂
1. 可以返回原返回类型的任何子类型的对象。我们在选择返回对象的类有更大的灵活性。



### 单例对象构建: 只创建1个对象.实现Singleton
1. 
    
```
public enum Elvis{
    INSTANCE;
}
```
### package
1. 基本说明
+ package语句作为Java源文件的第一条语句。(若缺省该语句，则指定为无名包。) 约定俗成的给包起名为把公司域名倒过来写，如com.sun
+ Java编译器把包对应于文件系统的目录管理，package语句中，用‘.’来指明包（目录）的层次，例如`package com.sun;`则该文件中所有的类位于`.\com\sun`目录下
2. import
    1. complete name of the class
    ```
    java.time.LocalDate today = java.time.LocalDate.now();
    ```
    2. 使用import语句: `import java.time.*`的语法比较简单，对代码的大小也没有任何负面的影响。但如果能明确指出导入的类，可以让读者明确知道加载了哪个类.
    ```
    import java.time.*;
    LocalDate today = LocalDate.now();
    ```
    3. 只导入包中的特定类
    ```
    import java.time.LocalDate;
    // ...    
    System.out.println(LocalDate.now());
    ```
    4. static
    ```
    import static java.lang.System.out;
    // ...
    out.println("test");
    ```
3. C++
+ C++程序员会将import与#include弄混。其实，两者并没有共同之处。
+ C++中需要用#include将外部声明加载进来，这是因为C++编译器只能查看正在编译的文件和include的文件；而java编译器可以查看其他文件，只要告诉它到哪里去查看就可以了。
+ 在Java中，显式给出包名时（如java.util.Date），则不需要import；而C++中无法避免用#include。
+ Java中，package与import类似于C++中的namespace和using.

4. 静态导入: 什么是静态导入?
    1. import不仅可以导入类，还可以导入静态方法和静态域。
    `import static java.lang.System.*;`就可以使用System类的静态方法和静态域了。
    ```
    out.println(“Hello World!”); //System.out
    exit(0); //System.exit
    ```
    2. 也可以导入特定的方法或域
    `import static java.lang.System.out;`


5. 将类放入包中
要将类放到包中，必须将包的名字放在源文件的开始，例如：
```
package com.horstmann.corejava
public class Employee
{
…
}
```
如果没有放置package语句，则这个类被放置在默认包（default package)中。


需要将类文件切实安置到其所归属之Package所对应的相对路径下。
```
package com.horstmann.corejava
public class Employee
{
…
}
```
需要将Employee.java放到`com/horstmann/corejava`目录下
编译器也会将class文件放到相同的目录结构中


6. 类的关系
+ association: 涉及其他类,2个类就是association
+ aggregation: 这两个对象有has的关系,比如城市和树
+ composition: 2个对象有包含关系,其中1个对象不能独立于另一个对象存在.比如人和心脏


7. Wrapper Class
    1. MAX_VALUE, MIN_VALUE
    2. Integer类内部包装了int类型,而Double类型内部封装了double
    3. Integer构造函数可以接收int或string
    4. Integer.valueOf => 构造一个指定值初始化的Integer类型
    5. unboxing: 用`+`处理2个Integer对象,那么就会unboxing成2个int
    6. BigInteger, BigDecimal => 2个都是不可修改的,内部有multiply/divede等成员方法
    7. String
        1. 构建
        ```
        String message = "hello"; // 如果使用这个方法构造,那么不同的对象都会引用相同的地址空间(放在常量池里面).可以使用==判断2个对象是否相同
        String message = new String("hello");
        String message = new String();
        ```
    8. boxing: Integer.valueOf        



8. Eclipse: create packages
    1. File > New > Package
    + Source folder: `hello/src`(for instance)
    + Name: `cn.test.java`
    2. click `cn.test.java` > New > Class
    + Source folder: `hello/src`
    + Package: package name, such as `cn.test.java`
    + Name: class name, `Test` for example
    3. click `Test.java` below `cn.test.java` and view the code. We find that `package cn.test.java;` is at the top of the codes, indicating this java file belong to the package called `cn.test.java`
    
    4. implement a static method called `print`
    
    5. return back to the main java file
    + `import cn.test.java.Test;` or `import cn.test.java.*`;  (package name + class name)
    + `Test.print()`




### 常量池技术
1. int
    1. 125 + new Integer(1) => 就是后者先intValue编程普通的int,然后相加,最后再valueOf
    2. IntegerCache为-127到128,所以超过范围的就不会被缓存
    3. `==`: 如果是int类型就是比较值是否相同,如果是Integer类型,则是比较引用是否相同
    4. Integer.valueOf会考虑缓存,而new Integer则不会,所以尽量使用前者
2. double没有缓存,因为太多了.所以Double.valueOf就是创建1个新的空间
3. string
    1. intern方法
