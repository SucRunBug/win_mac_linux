# Hadoop 实操文档

本文档基于章鱼大数据实验平台的实验，并不能深入了解Hadoop，但可以帮助您了解Hadoop的基本操作和简单的特性。

## shell 基本操作

- 启动Hadoop

`cd /apps/hadoop/sbin` 进入Hadoop的安装路径

`./start-all.sh`

- 查看启动状态

`jps`

- 终止hadoop

`./stop-all.sh`

- 创建目录

`hadoop fs -mkdir /mydir`

- 查看目录

`hadoop fs -ls -R /`

- 创建文件

`hadoop fs -touchz`

*z代表指定路径*

**总结为**：`hadoop fs -linux指令`

## shell 进阶操作

下面介绍一些进阶的指令：

前面命令的前缀均是hadoop fs，但这其实是早期版本的指令，现在版本建议使用hadoop dfs

- 将本地文件上传至HDFS

`hdfs dfs -put local_file_dir hdfs_dir`

- 查看文件数量

`hdfs dfs -count /dir`

- 查看文件大小

`hdfs dfs -du filename`

- 查看文件

`hdfs dfs -cat filename`

- 将hdfs文件下载到本地

`hdfs dfs -get hdfs_dir local_dir`

- 合并hdfs上的文档

`hdfs dfs -appendToFile src des`

## 使用Java API

使用Java API可以对部分上述命令进行复现。

以下代码均在主函数中：

首先是创建hdfs句柄

```java
Configuration conf = new Configuration();
String hdfsPath = "hdfs://localhost:9000";
FileSystem hdfs = FileSystem.get(new URI(hdfsPath), conf);
```

创建目录、创建文件、上传文件到Hadoop，从Hadoop下载文件到本地

```java
hdfs.mkdirs(new Path(newDir));	// newDir为String类型，表示要创建目录的路径
hdfs.create(new Path(filePath));
hdfs.copyFromLocalFile(new Path(form_linux), new Path(to_hdfs));
hdfs.copyToLocalFile(false, new Path(form_HDFS), new Path(to_linux));
```

列出HDFS文件系统/hdfstest目录下，所有的文件，以及文件的权限、用户组、所属用户。

核心接口listStatus

```java
String watchHDFS = "/hdfstest";  
FileStatus[] files = hdfs.listStatus(new Path(watchHDFS));  
for (FileStatus file : files) {  
	System.out.println(file.getPermission() + " " + file.getOwner()  
		+ " " + file.getGroup() + " " + file.getPath());  
}
```

递归遍历根目录下所有文件

```java
package my.hdfs;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URI;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
public class IteratorListFiles {
	public static void main(String[] args) throws IOException {
		Configuration conf = new Configuration();
		String hdfspath = "hdfs://localhost:9000/";
		FileSystem hdfs = FileSystem.get(URI.create(hdfspath), conf);
		String watchHDFS = "/";

		iteratorListFile(hdfs, new Path(watchHDFS));
	}
	public static void iteratorListFile(FileSystem hdfs, Path path)
			throws FileNotFoundException, IOException {
		FileStatus[] files = hdfs.listStatus(path);
		for (FileStatus file : files) {
			if (file.isDirectory()) {
				System.out.println(file.getPermission() + " " + file.getOwner()
						+ " " + file.getGroup() + " " + file.getPath());
				iteratorListFile(hdfs, file.getPath());
			} else if (file.isFile()) {
				System.out.println(file.getPermission() + " " + file.getOwner()
						+ " " + file.getGroup() + " " + file.getPath());
			}
		}
	}
}
```



## WordCount 词频统计

此条目不考虑环境配置，仅仅考虑编码部分。

大致思路是将hdfs上的文本作为输入，MapReduce通过InputFormat会将文本进行切片处理，并将每行的首字母相对于文本文件的首地址的偏移量作为输入键值对的key，文本内容作为输入键值对的value，经过在map函数处理，输出中间结果<word,1>的形式，并在reduce函数中完成对每个单词的词频统计。整个程序代码主要包括两部分：Mapper部分和Reducer部分。

完整代码

```java
package mapreduce;
import java.io.IOException;
import java.util.StringTokenizer;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
public class WordCount {
    public static void main(String[] args) throws IOException, ClassNotFoundException, InterruptedException {
        Job job = Job.getInstance();
        job.setJobName("WordCount");
        job.setJarByClass(WordCount.class);
        job.setMapperClass(doMapper.class);
        job.setReducerClass(doReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        Path in = new Path("hdfs://localhost:9000/mymapreduce1/in/buyer_favorite1");
        Path out = new Path("hdfs://localhost:9000/mymapreduce1/out");
        FileInputFormat.addInputPath(job, in);
        FileOutputFormat.setOutputPath(job, out);
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
//第一个Object表示输入key的类型；第二个Text表示输入value的类型；第三个Text表示输出键的类型；第四个IntWritable表示输出值的类型
    public static class doMapper extends Mapper<Object, Text, Text, IntWritable>{
        public static final IntWritable one = new IntWritable(1);
        public static Text word = new Text();
        @Override
        protected void map(Object key, Text value, Context context) throws IOException, InterruptedException {
            StringTokenizer tokenizer = new StringTokenizer(value.toString(), "\t"); //StringTokenizer是Java工具包中的一个类，用于将字符串进行拆分
            word.set(tokenizer.nextToken()); //返回当前位置到下一个分隔符之间的字符串
            context.write(word, one);	//将word存到容器中，记一个数
        }
    }
    //参数同Map一样，依次表示是输入键类型，输入值类型，输出键类型，输出值类型
    public static class doReducer extends Reducer<Text, IntWritable, Text, IntWritable>{
        private IntWritable result = new IntWritable();
        @Override
        protected void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
        	int sum = 0;
        	for (IntWritable value : values) { //for循环遍历，将得到的values值累加
        		sum += value.get();
        	}
        	result.set(sum);
        	context.write(key, result);
        }
    }
}
```



## MapReduce 去重

此条目不考虑环境配置，仅仅考虑编码部分。
