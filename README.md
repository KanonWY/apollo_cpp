## apollo_cpp

一个简单的apollo参数服务器的客户端，采用C++编写，支持基本的参数查询和参数设置，目前仅支持YAML格式和Properties格式的配置。
新增了一个headeronly的库，用于struct和yamlNode直接转换：  
[yaml_struct](https://github.com/KanonWY/yaml_struct) 
### 依赖

- cpprestsdk
- yaml-cpp
- spdlog

安装依赖

```bash
sudo apt-get install libcpprest-dev
sudo apt-get install libspdlog-dev
sudo apt-get install libyaml-cpp0.6
sudo apt-get install libyaml-cpp-dev
```

### 键值设计

参考了ros的参数服务器的设计，Key使用`/`来划分层级，value使用了`YAML::Node`来封装，并且value只能为`Scalar, Sequence`类型（与ros的设计一致）。

例如：testapp下的 test.yaml文件（如下）在客户端存储为：`<string,YAML::Node> =>  appid/namespace/namespace_inner_name  VALUE  `

```yaml
data_coverage_area:
  right_down: [4.5, 5.4]
  left_up: [3.5, 5.4]
  left_down: [2, 3]
  right_up: [1, 2]
data_start_record_time: 2011-11-11-11-11-11
data_collection_id: test1
```

存储结果为：

```bash
key																value
testapp/test.yaml/data_coverage_area/right_down					[4.5, 5.4]
testapp/test.yaml/data_coverage_area/left_up					[3.5, 5.4]
testapp/test.yaml/data_coverage_area/left_down					[2, 3]
testapp/test.yaml/data_coverage_area/right_up					[1, 2]
testapp/test.yaml/data_start_record_time						2011-11-11-11-11-11
testapp/test.yamldata_collection_id								test1
```

### 客户端使用

```cpp
#include "apollo_base_client.h"
// 1. 创建客户端
apollo_client::apollo_client_multi_ns_node client;
client.init(config);

// 2. 创建配置
apollo_client::MultiNsConfig config;
// 3. 设置基本配置，采用MultiNsConfig可以配置监听多个Ns.
config.SetAddress("http://localhost:8080").SetAppid("SampleApp").AppendNamespace("test_yaml.yaml").AppendNamespace("test_xml.xml");

// 4. 获取配置
auto res = cl.getConfigByKey("test_yaml.yaml/name");
```

### 开放平台客户端使用

普通客户端仅支持配置的查询操作，开放平台可以对配置进行修改和发布。

```
#include "apollo_openapi_base.h"

// 1. 创建开放平台客户端
apollo_client::apollo_ctrl_base client;

// 2. 创建配置，填写appid和特权地址
apollo_client::MultiNsConfig config;
config.SetAppid("openapp").SetAddress(Address);

// 3. 传入配置和token初始化
client.init(token, config);

// 4. 获取所有配置
auto res = client.getAllKey();
if(res.have_value())
{
	//....
}

// 5. 修改某一个配置
bool res = client.setConfig(key, value, namespaceName, appid);

// 6. 发布某一个配置
res  = client.publishNamespace(namespace, pubTitile);

// 7. 新增一个配置
res = client.addNewConfig(appid,namespace,key, value);

// 8. 删除一个配置
res = client.deleteConfig(appid, namespace, key,deleteUsrId);

// 9. 修改一个配置
res = client.setConfig(key, value, namespace, appid);
```

### CLI查询工具

apollo_cli支持对某一个某一个appid下的所有命名空间的配置key的查询。
