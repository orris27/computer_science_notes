## 数据组织
1. String
```
/** The value is used for character storage. */
private final char value[];

/** Cache the hash code for the string */
private int hash; // Default to 0

/** use serialVersionUID from JDK 1.0.2 for interoperability */
private static final long serialVersionUID = -6849794470754667710L;
```
String的主要成员是字符数组常量value、整形变量hash和长整型的serialVersionUID。Java使用String存储字符串的时候，每个字符都是保存在value成员里面的，根据Java数组的length可以得到字符串的常量，从而获得字符串的边界。需要注意的是，String中的value是final定义的，也就是说value成员是常量。也就是说我们不能修改value的值。虽然我们平时使用的`+=`乍看之下是直接在字符数组的后面添加新的字符，但实际在String中的实现并不是简单的写在后面。

String的value成员使用字符数组初始化时的代码是`this.value = Arrays.copyOf(value, value.length);`，利用了Arrays.copyOf方法，将参数的所有字符都拷贝到value成员中，从而实现初始化。


2. StringBuffer

在StringBuffer中，如果我们直接查看StringBuffer类的实现，我们只能看到serialVersionUID的成员，并不能像String一样看到value成员。所以我们可以推理，StringBuffer的value成员应该是继承过来的。
```
private static final long serialVersionUID = 3388685877147921107L;
```
查看StringBuffer的继承关系`public final class StringBuffer extends AbstractStringBuilder implements Appendable, Serializable, CharSequence`，可以看到StringBuffer是继承于AbstractStringBuilder的。查看AbstractStringBuilder的代码。可以看到下面的代码实现。

INITIAL_CAPACITY 表示初始化时value数组默认的长度

value 类似于String的value，也是字符数组，只是没有final定义

count int类型，记录字符串的长度

shared 布尔类型
```
static final int INITIAL_CAPACITY = 16;
private char[] value;
private int count;
private boolean shared;
```
首先，StringBuffer的value也是字符数组，但是没有final定义，也就是说StringBuffer的value是可变的，我们可以修改value的值。并且StringBuffer里多出了INITIAL_CAPACITY和count等成员，前者说明了StringBuffer默认创建的是16字节长度的value数组，而String的话默认是空字符串或根据构造函数的参数来决定自己的字符串长度，而StringBuffer则不然，StringBuffer在构造函数的参数的字符长度少于16时仍然构件16长度的字符数组，并且根据参数的字符串的大小动态决定自己的value长度。count成员用来标记字符串的长度，因为我们不能通过数组长度来决定字符串的长度，value数组后面的部分可能是无意义的，所以我们需要一个count成员来标记字符串的长度。在StringBuffer实例增加或减少字符的时候，count和value都会改变。


3. StringBuilder

在StringBuilder中，如果我们直接查看StringBuilder类的实现，同样只能看到serialVersionUID的成员。所以我们可以推理，StringBuilder的value成员和StringBuffer一样是继承过来的。
```
private static final long serialVersionUID = 3388685877147921107L;
```
查看StringBuilder的继承关系`public final class StringBuffer extends AbstractStringBuilder implements Appendable, Serializable, CharSequence`，可以看到StringBuilder也是继承于AbstractStringBuilder的。具体的数据组织和StringBuffer一样，不赘述。


## 功能实现



## 设计原因


## 影响

## 使用场景
