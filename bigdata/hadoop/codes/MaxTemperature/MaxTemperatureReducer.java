import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
 
public class MaxTemperatureReducer extends
		Reducer<Text, IntWritable, Text, IntWritable> {
 
	/**
 * 	 * reduce端的业务就比较简单了，因为通partion阶段同一年份的
 * 	 	 * 温度已经被归到同一个迭代器中（对于partion的部分后面会详解），
 * 	 	 	 * 所以reduce就只需要把筛选出
 * 	 	 	 	 * 最高的温度进行输出就行
 * 	 	 	 	 	 */
	@Override
	protected void reduce(Text key, Iterable<IntWritable> values,Context context)
			throws IOException, InterruptedException {
		int max_temp = Integer.MIN_VALUE;
		/**
 * 		 * 找出迭代器中传入年份的最高温度
 * 		 		 */
		for(IntWritable value : values) {
			max_temp = Math.max(max_temp, value.get());
		}
		/**
 * 		 * 输出年份 以及该年份的最高温度
 * 		 		 */
		context.write(key, new IntWritable(max_temp));
	}
}

