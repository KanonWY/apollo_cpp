### apollo参数客户端代理
#### 1. 功能：
- 1、维持与apollo服务器的长链接。
- 2、维持与所有需要使用参数的进程短连接或者长链接。
- 3、是否必要作服务器授权。
```bash
                  
         apollo server in clouder
                 /|\
                  |
                  |   appid1
                  |   appid2
                  |   appid3
                  |
              apollo_proxy
                  |
                  |
                  |
         appId for carA
         appId for PUBLIC config
                |
                |
                |
                |
                A appID |----------------> ns1
                        |----------------> ns2
                        |----------------> ns3
                 
                      
                |
                |
                |
                |
                |
                PUBLIC |-----------------> ns1p
                       |-----------------> ns2p
                       |-----------------> ns3p
```
