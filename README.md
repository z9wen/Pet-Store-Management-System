# **宠物店管理系统开发文档**

## 1. 项目概述

**项目名称**: 宠物店管理系统  
**开发语言**: C++  
**数据库**: SQLite（持久化存储）  
**缓存与消息队列**: Redis（含 Redis Streams）  
**前端**: HTML、CSS、JavaScript  
**目标**: 开发一个用于管理宠物店日常运营的管理系统，功能包括用户管理、订单管理、库存管理和消息通知。

---

## 2. 技术栈

### 2.1 后端
- **语言**: C++  
  - 用于实现主要的业务逻辑，如用户管理、订单处理、库存管理等。
  
- **SQLite**: 
  - 用于持久化存储，如存储用户数据、订单记录、库存信息。
  - 适合小型项目，轻量、易部署，但在高并发情况下有一定局限。

- **Redis**: 
  - 作为缓存数据库和消息队列使用。
  - 通过 **Redis Streams** 实现消息队列功能，处理异步任务和缓解 SQLite 并发性能瓶颈。

### 2.2 前端
- **HTML**:  
  - 用于构建用户界面，负责页面结构和内容展示。
  
- **CSS**:  
  - 用于美化页面，增强用户体验。
  
- **JavaScript**:  
  - 用于处理页面的动态交互以及前后端通信（AJAX 请求等）。
  
### 2.3 消息队列
- **Redis Streams**:  
  - 实现异步任务处理，用于订单处理、库存管理等后台操作，提升系统的并发性能。

### 2.4 后期扩展技术
- **Kafka**（可选）：  
  - 随着项目扩展，未来可能迁移到 Kafka 作为消息队列，处理更高量级的异步任务和日志管理。

- **MySQL**（可选）：  
  - 当数据量增长时，SQLite 的性能可能不足，未来可能迁移到 MySQL 来处理大规模数据及并发需求。

---

## 3. 系统架构

### 3.1 系统概述
系统采用 C++ 作为后端语言，通过 Redis Streams 实现异步任务处理和消息队列。前端使用 HTML、CSS 和 JavaScript 构建用户界面，前后端通过 API 进行通信。SQLite 用于存储持久化数据，Redis 作为缓存和消息队列，以缓解并发问题。

### 3.2 架构图

```plaintext
+--------------------------------------------------------+
|                        前端 UI                        |
|  (HTML, CSS, JavaScript - 用户界面与交互)              |
+---------------------------|----------------------------+
                            |
                            v
+--------------------------------------------------------+
|                        后端服务                       |
|  (C++ - 处理业务逻辑：用户、订单、库存等)             |
|                      + Redis Streams +                |
|                      +   缓存与消息队列              |
+---------------------------|----------------------------+
                            |
                            v
+--------------------------------------------------------+
|                       数据存储层                      |
|               SQLite (持久化存储)                     |
|                Redis (缓存与队列)                     |
+--------------------------------------------------------+
```

### 3.3 模块划分

#### 3.3.1 用户管理模块
- **功能**: 
  - 用户登录、注册、会话管理。
  - 用户信息的增删改查。
- **数据存储**: 
  - 使用 SQLite 存储用户信息，Redis 缓存用户的会话状态。

#### 3.3.2 订单管理模块
- **功能**: 
  - 处理用户的下单请求，生成订单，更新库存。
  - 异步处理订单确认、发货通知等。
- **数据存储**: 
  - 订单数据存储在 SQLite 中，订单生成后通过 Redis Streams 处理通知或其他异步操作。

#### 3.3.3 库存管理模块
- **功能**: 
  - 管理宠物店的商品库存，包括商品入库、出库和库存状态的更新。
  - 当订单创建时，异步更新库存信息。
- **数据存储**: 
  - 库存信息存储在 SQLite 中，库存更新通过 Redis Streams 异步处理。

#### 3.3.4 消息通知模块
- **功能**: 
  - 当订单生成、库存不足或重要操作发生时，系统通过 Redis Streams 异步发送通知。
- **实现**: 
  - Redis Streams 作为消息队列，处理不同类型的通知，并发送到用户的前端界面。

---

## 4. 数据库设计

### 4.1 SQLite 数据库

- **用户表**: 存储用户信息
  ```sql
  CREATE TABLE users (
      user_id INTEGER PRIMARY KEY AUTOINCREMENT,
      username TEXT NOT NULL,
      password TEXT NOT NULL,
      email TEXT NOT NULL,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP
  );
  ```

- **订单表**: 存储订单信息
  ```sql
  CREATE TABLE orders (
      order_id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER NOT NULL,
      total_price REAL NOT NULL,
      status TEXT NOT NULL,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
      FOREIGN KEY (user_id) REFERENCES users(user_id)
  );
  ```

- **库存表**: 存储库存信息
  ```sql
  CREATE TABLE inventory (
      product_id INTEGER PRIMARY KEY AUTOINCREMENT,
      product_name TEXT NOT NULL,
      quantity INTEGER NOT NULL,
      price REAL NOT NULL
  );
  ```

---

## 5. Redis Streams 使用

### 5.1 消息队列设计
Redis Streams 用于处理异步任务，如订单生成后通知处理、库存更新等。

- **订单生成队列**:
  - 订单生成时，订单信息写入 Redis Streams 队列，后台消费者从队列中消费订单，进行发货通知或其他操作。
  
- **库存更新队列**:
  - 用户下单后，库存更新操作写入 Redis Streams 队列，异步更新库存状态。

### 5.2 示例代码

```cpp
// 将订单信息写入 Redis Streams
void pushOrderToRedis(const std::string& orderData) {
    redisCommand(context, "XADD orders * order_data %s", orderData.c_str());
}

// 消费 Redis Streams 中的订单信息
void consumeOrdersFromRedis() {
    redisReply *reply = (redisReply*)redisCommand(context, "XREAD COUNT 1 STREAMS orders 0");
    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        std::string orderData = reply->element[0]->str;
        // 处理订单数据
        processOrder(orderData);
    }
    freeReplyObject(reply);
}
```

---

## 6. 前后端通信

前后端通过 REST API 进行通信。前端通过 JavaScript 发起请求，后端处理并返回 JSON 格式的响应。

### 6.1 示例 API 设计

- **登录 API**：
  - `POST /api/login`
  - 请求体: `{ "username": "user", "password": "pass" }`
  - 响应体: `{ "status": "success", "token": "xxx" }`

- **创建订单 API**：
  - `POST /api/orders`
  - 请求体: `{ "user_id": 1, "items": [...] }`
  - 响应体: `{ "status": "success", "order_id": 123 }`

---

## 7. 未来迭代与扩展计划

### 7.1 数据库迁移
- 当数据量增加时，可以将 SQLite 数据迁移到 MySQL，以提高并发处理能力和数据库的可扩展性。

### 7.2 消息队列扩展
- 初期使用 Redis Streams 处理异步任务，未来当系统需求更复杂时，可以逐步迁移到 Kafka 以处理更大规模的消息队列。

### 7.3 容器化部署
- 未来计划使用 Docker 容器化后端服务、数据库（Redis、SQLite/MySQL）及前端项目，便于部署和管理。

---

## 8. 开发与测试流程

### 8.1 开发工具
- **编译器**: GCC / Clang
- **集成开发环境（IDE）**: Visual Studio Code / CLion
- **版本控制**: Git + GitHub / GitLab
- **Redis 客户端**: `hiredis`
- **测试框架**: Google Test / Catch2

接下来是开发文档剩余部分的补充，包括**性能测试**、**部署与运维**、**故障排查与监控**、以及项目的未来迭代方向等。

---

### 8.2 测试流程

- **性能测试**：
  - 使用工具：可以使用 `Apache JMeter` 或 `wrk` 对系统的关键 API（如订单创建、库存查询等）进行压力测试。
  - 测试目标：测量系统在高并发场景下的响应时间和处理能力。特别要关注 Redis Streams 的消息处理性能和 SQLite 的查询性能。
  - 测试内容：
    1. 测试 API 的响应时间（如订单创建、库存查询等）。
    2. 测试 Redis Streams 的消息消费速度和 Redis 缓存命中的情况。
    3. 测试高并发情况下 SQLite 读写操作的性能瓶颈。
    4. 测试系统在处理并发订单请求时的 CPU 和内存占用情况。

- **负载测试**：
  - 模拟大量用户同时访问和下单，观察系统的行为，分析系统在高负载下的表现。
  - Redis Streams 的消息积压处理情况测试，验证系统的削峰填谷能力。

- **容错测试**：
  - 通过模拟数据库故障、缓存失效、网络抖动等情况，观察系统如何恢复正常。
  - 验证消息队列（Redis Streams）在消费者故障时的消息恢复机制。

---

### 9. 部署与运维

### 9.1 部署架构

系统的各个组件可以根据需求分离部署：

- **后端服务**：
  - C++ 后端服务负责处理用户请求、业务逻辑执行。
  - 部署在 Linux 环境下，使用 Docker 容器化可以更好地管理部署。

- **数据库与缓存**：
  - **SQLite**：适用于单节点轻量部署，无需单独服务器。
  - **Redis**：用作缓存和消息队列，可以通过 Docker 部署单节点 Redis 实例。

- **前端服务**：
  - HTML、CSS、JavaScript 静态文件可直接托管在 Nginx 或 Apache 服务器中。

### 9.2 部署步骤

1. **准备环境**：
   - 确保服务器安装了 Docker、Redis 和 SQLite。
   - 如果使用 MySQL 作为后续数据库扩展，也需要安装 MySQL。

2. **编译 C++ 后端**：
   - 使用 `g++` 或 `clang++` 编译 C++ 代码，并将编译后的二进制文件部署到服务器。

3. **配置 Redis**：
   - 部署 Redis，确保 Redis Streams 功能可用。
   - 配置 Redis 缓存的最大内存占用和持久化策略。

4. **部署前端**：
   - 将前端静态资源部署到 Nginx/Apache，并配置负载均衡（如有需要）。

5. **启动系统**：
   - 通过 Docker 启动 Redis 和其他依赖服务。
   - 启动后端服务，确保 API 正常工作。

### 9.3 持续集成与交付（CI/CD）

- **GitHub Actions / GitLab CI**：
  - 设置持续集成流程，确保每次代码提交后自动执行单元测试和集成测试。
  - 使用 Docker 镜像部署流程，在每次构建后生成新的 Docker 镜像并推送到 Docker Registry。

- **自动化测试**：
  - 集成 Google Test / Catch2 自动化测试框架，确保每个功能模块在修改后可以自动测试。

---

### 10. 监控与故障排查

### 10.1 系统监控

- **Prometheus + Grafana**：
  - 使用 Prometheus 采集 Redis、SQLite 以及系统本身的性能数据，包括 CPU、内存、网络流量、Redis Streams 消费速度等。
  - Grafana 提供可视化的系统监控面板，便于实时观察系统状态和性能。

- **Redis 内存使用监控**：
  - 使用 Redis 内置的监控命令 `INFO`，持续监控 Redis 的内存占用、缓存命中率和消息积压状态。
  - 在 Redis Streams 消息处理高峰期，监控消息队列长度，防止队列溢出。

- **日志管理**：
  - 通过 ELK Stack（Elasticsearch、Logstash、Kibana）集中管理日志。
  - 记录后端服务的请求日志、Redis 缓存日志和数据库操作日志，并通过 Kibana 分析系统运行情况。

### 10.2 故障排查

- **Redis 故障处理**：
  - 如果 Redis 服务异常，使用持久化策略（如 AOF 或 RDB）恢复 Redis 数据。
  - 监控 Redis 消息消费进度，确保消费服务在异常恢复后能够继续处理积压的消息。

- **SQLite 性能问题**：
  - 在高并发写入时，如果遇到性能瓶颈，考虑优化 SQLite 索引或切换到 MySQL。

- **服务异常重启**：
  - 使用 Supervisor 或 systemd 监控 C++ 后端服务，确保服务崩溃后能够自动重启。

---

### 11. 未来迭代与扩展计划

### 11.1 数据库扩展

- **迁移到 MySQL**：
  - 当数据量增大时，将 SQLite 数据迁移到 MySQL。
  - 使用 MySQL 的事务管理和并发处理能力提升系统性能。

- **分布式数据库**：
  - 随着系统扩展，可以考虑使用分布式数据库如 CockroachDB 或 YugabyteDB，以处理更大规模的数据。

### 11.2 消息队列扩展

- **Kafka 集成**：
  - 如果 Redis Streams 的消息处理能力不足，可以将消息队列迁移到 Kafka，处理大规模数据流。
  - Kafka 可以用于实时分析、日志管理和高并发的异步任务处理。

### 11.3 服务容器化与集群化

- **Kubernetes 部署**：
  - 随着系统规模扩展，可以将整个系统容器化，部署到 Kubernetes 集群上。
  - 自动进行服务的弹性伸缩，确保高并发和高可用性。

### 11.4 前端扩展

- **前端框架升级**：
  - 随着系统复杂度的提升，可以使用现代前端框架（如 React、Vue.js 或 Svelte）重构前端，提升用户体验和开发效率。

---

## 12. 结语

本开发文档提供了宠物店管理系统的架构、技术栈、模块设计、数据库结构、消息队列使用等方面的详细说明。初期系统将使用 C++ 作为后端，SQLite 和 Redis 作为数据库与缓存，Redis Streams 处理异步任务。未来可以逐步扩展为分布式系统，使用 Kafka 和 MySQL，满足大规模并发处理和更复杂的业务需求。

