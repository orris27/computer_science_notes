
import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
/**
 *  0067011990999991950051507004888888880500001N9+00781+9999999999999999999999
 *	数据说明： 
 *	第15-19个字符是year
 *	第45-50位是温度表示，+表示零上 -表示零下，且温度的值不能是9999，9999表示异常数据
 *	第50位值只能是0、1、4、5、9几个数字
 * @author Administrator
 */
public class MaxTemperatureMapper extends Mapper<LongWritable, Text, Text, IntWritable> {
	/**
	 * 定义异常数据常量
	 */
	private static final int FAIL_DATA = 9999;
	
	@Override
	protected void map(LongWritable key, Text value, Context context)
			throws IOException, InterruptedException {
		String line = value.toString();
		String year = line.substring(15, 19);
		/**
		 * 取出温度的正负号
		 */
		String tmp = line.substring(45, 46);
		int temp = 0;
		/**
		 * 判断温度为正还是负并在原始串中取出值
		 */
		if( "+".equals(tmp)) {
			temp = Integer.valueOf(line.substring(46, 50));
		} else {
			temp = Integer.valueOf(line.substring(45, 50));
		}
		/**
		 * 判断温度是否异常，且第50位的标志位是否正常
		 */
		if( temp != FAIL_DATA && line.substring(50, 51).matches("[01459]")){
			/**
			 * 所有判断通过则输出年 温度到Reduce
			 */
			context.write(new Text(year), new IntWritable(temp));
		}
	}
	
	/**
	 * 一般写Mapreduce时都可以假设数据是以什么形式进入map或者reduce以便
	 * 检验逻辑是否正确
	 * @param args
	 */
	public static void main(String[] args) {
		String line = "0067011990999991950051507004888888880500001N9-99991+9999999999999999999999";
		String year = line.substring(15, 19);
		String tmp = line.substring(45, 46);
		int temp = 0;
		if( "+".equals(tmp)) {
			temp = Integer.valueOf(line.substring(46, 50));
		} else {
			temp = Integer.valueOf(line.substring(45, 50));
		}
		if( Integer.valueOf(line.substring(46, 50)) != FAIL_DATA && line.substring(50, 51).matches("[01459]")){
			System.out.println(year + " " + temp);
		}
	}
}
