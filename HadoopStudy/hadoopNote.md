## shell基本操作

- 启动Hadoop

`cd /apps/hadoop/sbin`

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



## 使用Java API

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

