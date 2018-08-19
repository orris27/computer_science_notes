import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
 
 
public class JobMain {
	public static void main(String[] args) throws Exception {
		Configuration configuration = new Configuration();
		Job job = new Job(configuration,"max_temp_job");
		job.setJarByClass(JobMain.class);
		job.setMapperClass(MaxTemperatureMapper.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(IntWritable.class);
		job.setReducerClass(MaxTemperatureReducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(IntWritable.class);
		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileSystem fs = FileSystem.get(configuration);
		Path outputDir = new Path(args[1]);
		if(fs.exists(outputDir)) {
			fs.delete(outputDir,true);
			System.out.println("the outputDir is exist,but it has been deleteed!");
		}
		FileOutputFormat.setOutputPath(job, outputDir);
		System.exit(job.waitForCompletion(true)? 0: 1);
	}
}

