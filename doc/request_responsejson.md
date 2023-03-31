## 配置的类型
在apollo中配置的类型主要五类：
- properties
- xml
- json
- yaml
- txt

使用开放平台去获取数据，返回的数据样式大致分为两类。（详细实例见下述的代码块中）

1、访问properties类型的命名空间的配置：此类配置是键值对类型。返回值中可以指导总共的键值对的数量。

2、非properties类型的命名空间的配置：此类型默认返回的是字符串类型（在返回的value）字段中。


### 1、使用开放平台请求数据：

- properties请求
```bash
http://localhost:8070/openapi/v1/envs/DEV/apps/openapp/clusters/default/namespaces/application/items

# 返回JSON数据
{
    "page": 0,
    "size": 50,
    "total": 3,
    "content": [
        {
            "key": "prokey",
            "type": 0,
            "value": "provalue",
            "dataChangeCreatedBy": "apollo",
            "dataChangeLastModifiedBy": "apollo",
            "dataChangeCreatedTime": "2023-03-31T15:23:24.000+0800",
            "dataChangeLastModifiedTime": "2023-03-31T15:23:24.000+0800"
        },
        {
            "key": "test3",
            "type": 0,
            "value": "dadad",
            "dataChangeCreatedBy": "apollo",
            "dataChangeLastModifiedBy": "apollo",
            "dataChangeCreatedTime": "2023-03-31T16:41:39.000+0800",
            "dataChangeLastModifiedTime": "2023-03-31T16:41:39.000+0800"
        },
        {
            "key": "dadad",
            "type": 1,
            "value": "2000",
            "dataChangeCreatedBy": "apollo",
            "dataChangeLastModifiedBy": "apollo",
            "dataChangeCreatedTime": "2023-03-31T16:43:12.000+0800",
            "dataChangeLastModifiedTime": "2023-03-31T16:43:12.000+0800"
        }
    ]
}
```
- yaml请求
```bash

http://localhost:8070/openapi/v1/envs/DEV/apps/openapp/clusters/default/namespaces/testyaml.yaml/items/content

{
    "key": "content",
    "type": 0,
    "value": "yamlkey: value1\nyamlkey2: value2\nyamlkey3: value3",
    "dataChangeCreatedBy": "apollo",
    "dataChangeLastModifiedBy": "apollo",
    "dataChangeCreatedTime": "2023-03-31T15:23:02.000+0800",
    "dataChangeLastModifiedTime": "2023-03-31T16:32:55.000+0800"
}
```

- xml请求
```bash
http://localhost:8070/openapi/v1/envs/DEV/apps/openapp/clusters/default/namespaces/testxml.xml/items/content
{
    "key": "content",
    "type": 0,
    "value": "<hello>\n    hello\n</hello>",
    "dataChangeCreatedBy": "apollo",
    "dataChangeLastModifiedBy": "apollo",
    "dataChangeCreatedTime": "2023-03-31T16:47:44.000+0800",
    "dataChangeLastModifiedTime": "2023-03-31T16:47:44.000+0800"
}
```


- yml请求
```bash

http://localhost:8070/openapi/v1/envs/DEV/apps/openapp/clusters/default/namespaces/testyml.yml/items/content

{
    "key": "content",
    "type": 0,
    "value": "key1: value1\nkey2: value2",
    "dataChangeCreatedBy": "apollo",
    "dataChangeLastModifiedBy": "apollo",
    "dataChangeCreatedTime": "2023-03-31T16:50:05.000+0800",
    "dataChangeLastModifiedTime": "2023-03-31T16:50:05.000+0800"
}
```


- json请求
```bash
http://localhost:8070/openapi/v1/envs/DEV/apps/openapp/clusters/default/namespaces/testjson.json/items/content
{
    "key": "content",
    "type": 0,
    "value": "{\"hello\":\"kokokoko\"}",
    "dataChangeCreatedBy": "apollo",
    "dataChangeLastModifiedBy": "apollo",
    "dataChangeCreatedTime": "2023-03-31T16:51:30.000+0800",
    "dataChangeLastModifiedTime": "2023-03-31T16:51:30.000+0800"
}
```
- txt request
```bash
http://localhost:8070/openapi/v1/envs/DEV/apps/openapp/clusters/default/namespaces/testtxt.txt/items/content

{
    "key": "content",
    "type": 0,
    "value": "This is a testtxt!",
    "dataChangeCreatedBy": "apollo",
    "dataChangeLastModifiedBy": "apollo",
    "dataChangeCreatedTime": "2023-03-31T16:52:54.000+0800",
    "dataChangeLastModifiedTime": "2023-03-31T16:52:54.000+0800"
}
```

### 2、使用开放平台修改配置

