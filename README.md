## apollo_cpp

一个简单参数服务器apollo的cpp客户端，支持C++11。http-client库采用`cpprestsdk::cpprest`(
使用该库之后就不需要额外引入json解析库以及http相关的库，可以简单满足；apollo客户端的相关http请求。支持阻塞与非阻塞调用以及超时等待，满足配置自动更新监测所需的相关API)
，日志库使用了`spdlog`，为了解析`yaml`格式，引入了`libyaml-cpp0.6`。
参考了ros的参数服务器方式，使用\对Yaml的各个层级进行划分。

- 安装依赖

```bash
# install cpprest and spdlog yaml-cpp
sudo apt-get install libcpprest-dev
sudo apt-get install libspdlog-dev
sudo apt-get install libyaml-cpp0.6
sudo apt-get install libyaml-cpp-dev
# 如果还要编译proxy，还需要安装pb和asio
sudo apt-get install protobuf-compiler libprotobuf-dev
sudo apt install libasio-dev
```

- 简单使用

```cpp
// 设置配置
apollo_client::MultiNsConfig ns;
ns.SetAddress("http://localhost:8080").SetAppid("SampleApp").AppendNamespace("test_yaml.yaml").AppendNamespace("test_xml.xml");
apollo_client::apollo_client_multi_ns cl;
//初始化
cl.init(ns);
//获取接口
auto res = cl.getConfigByKey("key1");
std::cout << "res = " << res.c_str() << std::endl;
while (true) {     
    std::this_thread::sleep_for(std::chrono::microseconds(100));
}
```