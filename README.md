## 1. apollo_cpp概述

一个简单参数服务器apollo的cpp客户端，支持C++11。http-client库采用`cpprestsdk::cpprest`.

[cpprest example](http://www.atakansarioglu.com/easy-quick-start-cplusplus-rest-client-example-cpprest-tutorial/)

使用该库之后就不需要额外引入json解析库以及http相关的库，可以简单满足；apollo客户端的相关http请求。支持阻塞与非阻塞调用以及超时等待，满足配置
自动更新监测所需的相关API。

## 2. apollo客户端获取配置与更新推送

### 2.1 配置更新推送

1、 基本请求与返回

基本请求：

```bash
URL: {config_server_url}/notifications/v2?appId={appId}&cluster={clusterName}&notifications={notifications}

Method: GET
```
2、基本流程图

```mermaid
sequenceDiagram
客户端 ->> Apollo远程服务: 请求远端服务【GET (appid，clusterName，notifications）】
Apollo远程服务 ->> 客户端: 返回JSON数据（变化的namespace和最新的notificationId）
```

3、配置更新图

在客户端存在一个持续请求的任务，该任务主要是为了定期从配置服务器中获取最新的配置信息。

```mermaid
sequenceDiagram
客户端 ->> Apollo远端服务: GET 请求远端服务，带上自己的应用信息和通知信息
Apollo远端服务 ->> Apollo远端服务: check msg from client.如果都是新的，保持住请求60s
Apollo远端服务 -->> 客户端: 如果60s内没有变化，就在60秒时返回 HttpStatus 304
Apollo远端服务 -->> 客户端: 如果60秒内失效或者立即实效，则返回有效的JSON数据，HttpStatus 200
客户端 -->> 客户端: 客户端判断返回status, 200:拉取非缓存新数据，更新notifications
客户端 -->> 客户端: 客户端判断返回status, 304:配置没有变化，回到起始继续操作
```

4、返回JSON数据说明：

- 针对200OK，返回的JSON数据实例为：

```json
[
    {
        "namespaceName": "p1_pram_set",
        "notificationId": 16,
        "messages": {
            "details": {
                "cp1+default+p1_pram_set": 16
            }
        }
    },
    {
        "namespaceName": "p2_pram_set",
        "notificationId": 17,
        "messages": {
            "details": {
                "cp1+default+p2_pram_set": 17
            }
        }
    }
]
```

- 对于304，服务器会与客户端保持连接60s，然后返回304

- 如果其他错误，说明是请求的url格式有问题，注意传递参数需要使用url编码。[在线URL转换工具](https://tool.chinaz.com/tools/urlencode.aspx)

## 3. API设计

#### 3.1 接入需求

如何让c++的代码接入Apoll参数服务器 ，验证支持的参数类型：基本类型、嵌套、vector、list之类的。

answer:

目前apollo的版本是2.1.0，暂时只支持string类型的参数值，如果需要多个类型的需要自己解析字符串，然后拼接规则。
apollo已经有计划接入字段类型，但是还没有正式引入。具体[dis描述](https://github.com/apolloconfig/apollo/discussions/4716)。


#### 3.2 场景分析

一个自动驾驶的车辆，里面有很多工控机，每个工控机下有很多程序，每个程序都有它自己要关心的参数，如何去设计比较好。
1：以一个车为单位，如何管理这个车内所有工控机内程序的参数。
2：每个程序如何限制获取参数的权限，让它只能拿到程序关心的参数，而无法拿到其他程序的参数。
3：结合Apollo服务器的参数管理，是否可支持这种树状的参数管理。
4：对于不同的项目地，可能需要部署一个参数服务器，这个项目地下有多个车，而且这个项目地可能没有网络，如果有一些参数要更新（假设在北京更新），这个流程应该是怎么样的。

大概模型如下：

                 a car
            /		\        \
          / 		  \        \
        cp1		      cp2       cp3
        |    \     
        |      \
        p1      p2  ...
        |        |
    	|        |
    param set1  param set2
      /|\
       |
    |||||||
    apollo
- cp - 工控机
- p1 - 进程1
- param set1- 参数集

目前考虑有以下两种方式：

##### 方式1：

一个车就是对应一个应用。那么使用命名空间来区分不同工控机的不同进程的配置。

比如：

假设有一个车A，其应用名称为：app_car_a。

假设车A有三个工作控机Q、M、S。

假设Q下面有程序1，程序2，程序3；M下面有程序1，程序2，程序3；S下面有程序1，程序2，程序3。

那么在app_car_a应用下面就有9个命名空间。

```bash
appid: app_car_a,
namespace:[
      Q_process_1
      Q_process_2，
      Q_process_3，

      M_process_1，
      M_process_2，
      M_process_3，

      S_process_1，
      S_process_2，
      S_process_3，
]
```

##### 方式2：

一个车有多个应用，每车的每一个工控机对应一个应用。

假设有一个车B，它有以下三个工控机：Q、M、S。

每一个工控机下有一个多个程序1、2、3...

那么配置如下：

```bash
appid: app_car_b_Q,
namespace: [
    PROCESS_1
    PROCESS_2
    PROCESS_3
    ......
]

appid: app_car_b_M,
namespace: [
    PROCESS_1
    PROCESS_2
    PROCESS_3
    ......
]

appid: app_car_b_S,
namespace: [
    PROCESS_1
    PROCESS_2
    PROCESS_3
    ......
]
```
### 基本结构体
```bash
apoll_base用于最基础的一次连接需求的实现，里面应该不含有任何成员变量。
apollo_client继承于apollo_base可以使用底层接口封装不同命名空间与配置map的需求。
最好支持配置更新回调。
```
