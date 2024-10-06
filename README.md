# **Pet Store Management System Development Document**

## 1. Project Overview

**Project Name**: Pet Store Management System  
**Programming Language**: C++  
**Database**: SQLite (for persistent storage)  
**Cache & Message Queue**: Redis (including Redis Streams)  
**Frontend**: HTML, CSS, JavaScript  
**Goal**: Develop a system for managing the daily operations of a pet store, including user management, order management, inventory management, and notification handling.

---

## 2. Tech Stack

### 2.1 Backend
- **Language**: C++
  - Responsible for core business logic, such as user management, order processing, and inventory management.

- **PostgreSQL**:
  - Used for persistent data storage such as user information, order records, and inventory data.
  - PostgreSQL is a powerful, open-source relational database management system that supports advanced features such as ACID compliance, high concurrency, and scalability, making it more suitable for medium to large projects or applications with higher data volume or complexity.
  - Provides robust support for transactions and concurrent access, addressing limitations that SQLite might have in handling multiple simultaneous database operations.

- **Redis**:
  - Acts as both a cache database and message queue.
  - Redis Streams will be used for message queuing, handling asynchronous tasks, and complementing PostgreSQL's performance by offloading some real-time processing and data caching.
  - Redis helps reduce the load on PostgreSQL by caching frequently accessed data and managing message-based communication between services.

### 2.2 Frontend
- **HTML**:  
  - Used for building the structure and content of the user interface.

- **CSS**:  
  - Used for styling the pages and enhancing the user experience.

- **JavaScript**:  
  - Handles dynamic interactions and front-to-backend communication (e.g., AJAX requests).

### 2.3 Message Queue
- **Redis Streams**:  
  - Handles asynchronous task processing such as order processing, inventory management, and backend operations, improving system concurrency.

### 2.4 Future Extensions
- **Kafka** (optional):  
  - As the project grows, you may migrate to Kafka as the message queue to handle higher volumes of asynchronous tasks and log management.

- **MySQL** (optional):  
  - If the data volume grows and SQLite’s performance becomes inadequate, you can migrate to MySQL for better concurrency handling and scalability.

---

## 3. System Architecture

### 3.1 System Overview
The system uses C++ as the backend language, with Redis Streams handling asynchronous task processing and message queuing. The frontend is built using HTML, CSS, and JavaScript, with communication to the backend via APIs. SQLite is used for persistent storage, and Redis acts as both cache and message queue to handle concurrency and speed up operations.

### 3.2 Architecture Diagram

```plaintext
+--------------------------------------------------------+
|                        Frontend UI                    |
|  (HTML, CSS, JavaScript - User Interface & Interaction)|
+---------------------------|----------------------------+
                            |
                            v
+--------------------------------------------------------+
|                        Backend Service                |
|  (C++ - Handles Business Logic: Users, Orders, Inventory)|
|                      + Redis Streams +                |
|                      +   Cache & Message Queue        |
+---------------------------|----------------------------+
                            |
                            v
+--------------------------------------------------------+
|                       Data Storage Layer              |
|               SQLite (Persistent Storage)             |
|                Redis (Cache & Queue)                  |
+--------------------------------------------------------+
```

### 3.3 Module Breakdown

#### 3.3.1 User Management Module
- **Features**: 
  - User login, registration, session management.
  - CRUD operations for user information.
- **Data Storage**: 
  - Uses SQLite for storing user data, and Redis for caching session data.

#### 3.3.2 Order Management Module
- **Features**: 
  - Handles user order requests, generates orders, updates inventory.
  - Asynchronously processes order confirmation and shipping notifications.
- **Data Storage**: 
  - Order data is stored in SQLite, and after order generation, order info is pushed to Redis Streams for asynchronous tasks like notifications.

#### 3.3.3 Inventory Management Module
- **Features**: 
  - Manages store inventory, including product intake, outgoing, and inventory status updates.
  - When an order is created, the inventory is updated asynchronously.
- **Data Storage**: 
  - Inventory data is stored in SQLite, and updates are handled via Redis Streams asynchronously.

#### 3.3.4 Notification Module
- **Features**: 
  - Sends notifications when orders are created, inventory is low, or important actions occur.
- **Implementation**: 
  - Redis Streams serve as the message queue, handling different types of notifications and sending them to the front-end.

---

## 4. Database Design

### 4.1 PostgreSQL Database

- **Products Table**: Stores product information.
  ```sql
    CREATE TABLE IF NOT EXISTS Products (
        product_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each product
        name TEXT NOT NULL,  -- Name of the product
        price NUMERIC(10, 2) NOT NULL,  -- Price of the product with two decimal precision
        stock INTEGER NOT NULL,  -- Current stock quantity of the product
        category TEXT,  -- Product category or classification
        is_deleted BOOLEAN DEFAULT FALSE  -- Soft delete flag, marks whether the product is logically deleted
    );
  ```

- **Employees Table**: Stores employee information.
  ```sql
    CREATE TABLE IF NOT EXISTS Employees (
        employee_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each employee
        name TEXT NOT NULL,  -- Name of the employee
        position TEXT NOT NULL,  -- Job position of the employee
        hire_date DATE NOT NULL,  -- Date when the employee was hired
        contact_info TEXT,  -- Contact details of the employee
        is_deleted BOOLEAN DEFAULT FALSE  -- Soft delete flag, marks whether the employee is logically deleted
    );
  ```

- **Orders Table**: Stores order information.
  ```sql
    CREATE TABLE IF NOT EXISTS Orders (
        order_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each order
        order_date DATE NOT NULL,  -- Date when the order was placed
        employee_id INTEGER,  -- Reference to the employee who handled the order (foreign key to Employees table)
        customer_id INTEGER,  -- Reference to the customer who placed the order (foreign key to Customers table)
        total NUMERIC(10, 2) NOT NULL,  -- Total amount of the order, with two decimal precision
        status TEXT NOT NULL,  -- Status of the order (e.g., pending, completed)
        is_deleted BOOLEAN DEFAULT FALSE,  -- Soft delete flag, marks whether the order is logically deleted
        FOREIGN KEY (employee_id) REFERENCES Employees(employee_id),  -- Foreign key to Employees table
        FOREIGN KEY (customer_id) REFERENCES Customers(customer_id)  -- Foreign key to Customers table
    );
  ```

- **Order_Items Table**: Stores order items (many-to-many relationship between orders and products).
  ```sql
    CREATE TABLE IF NOT EXISTS Order_Items (
        order_item_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each order item
        order_id INTEGER NOT NULL,  -- Reference to the order (foreign key to Orders table)
        product_id INTEGER NOT NULL,  -- Reference to the product in the order (foreign key to Products table)
        quantity INTEGER NOT NULL,  -- Quantity of the product ordered
        price NUMERIC(10, 2) NOT NULL,  -- Price of the product at the time of order, with two decimal precision
        is_deleted BOOLEAN DEFAULT FALSE,  -- Soft delete flag, marks whether the order item is logically deleted
        FOREIGN KEY (order_id) REFERENCES Orders(order_id),  -- Foreign key to Orders table
        FOREIGN KEY (product_id) REFERENCES Products(product_id)  -- Foreign key to Products table
    );
  ```

- **Customers Table**: Stores customer information.
  ```sql
    CREATE TABLE IF NOT EXISTS Customers (
        customer_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each customer
        name TEXT NOT NULL,  -- Name of the customer
        phone_number TEXT NOT NULL,  -- Customer's phone number
        email TEXT NOT NULL,  -- Customer's email address
        address TEXT NOT NULL,  -- Customer's address
        is_deleted BOOLEAN DEFAULT FALSE  -- Soft delete flag, marks whether the customer is logically deleted
    );
  ```

- **Suppliers Table**: Stores supplier information.
  ```sql
    CREATE TABLE IF NOT EXISTS Suppliers (
        supplier_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each supplier
        name TEXT NOT NULL,  -- Name of the supplier
        contact_info TEXT,  -- Supplier's contact details
        product_id INTEGER,  -- Reference to the product supplied by this supplier (foreign key to Products table)
        is_deleted BOOLEAN DEFAULT FALSE,  -- Soft delete flag, marks whether the supplier is logically deleted
        FOREIGN KEY (product_id) REFERENCES Products(product_id)  -- Foreign key to Products table
    );
  ```

- **Inventory_Actions Table**: Tracks inventory actions (stock changes).
  ```sql
    CREATE TABLE IF NOT EXISTS Inventory_Actions (
        action_id SERIAL PRIMARY KEY,  -- Auto-incrementing unique identifier for each inventory action
        product_id INTEGER NOT NULL,  -- Reference to the product for the inventory action (foreign key to Products table)
        action_type TEXT NOT NULL,  -- Type of inventory action (e.g., inbound, outbound)
        quantity INTEGER NOT NULL,  -- Quantity of the inventory action
        action_date DATE NOT NULL,  -- Date when the inventory action took place
        is_deleted BOOLEAN DEFAULT FALSE,  -- Soft delete flag, marks whether the inventory action is logically deleted
        FOREIGN KEY (product_id) REFERENCES Products(product_id)  -- Foreign key to Products table
    );
  ```

---

## 5. Redis Streams Usage

### 5.1 Message Queue Design
Redis Streams will be used to handle asynchronous tasks, such as order generation notifications, inventory updates, etc.

- **Order Generation Queue**:
  - When an order is created, the order information is written to the Redis Streams queue. A background consumer processes these orders and handles tasks like notifications.

- **Inventory Update Queue**:
  - After an order is placed, the inventory update task is written to the Redis Streams queue, and the inventory status is updated asynchronously.

---

## 6. Frontend-Backend Communication

The frontend communicates with the backend via REST APIs. The frontend sends requests through JavaScript and receives responses in JSON format.

### 6.1 Example API Design

- **Login API**:
  - `POST /api/login`
  - Request body: `{ "username": "user", "password": "pass" }`
  - Response body: `{ "status": "success", "token": "xxx" }`

- **Create Order API**:
  - `POST /api/orders`
  - Request body: `{ "user_id": 1, "items": [...] }`
  - Response body: `{ "status": "success", "order_id": 123 }`

---

## 7. Future Iterations & Extensions

### 7.1 Database Migration
- As the data volume increases, SQLite can be migrated to MySQL for better concurrency handling and scalability.

### 7.2 Message Queue Extension
- Initially, Redis Streams will handle asynchronous tasks. As the system grows more complex, you can migrate to Kafka to handle larger-scale message queues.

### 7.3 Containerization
- Plan to containerize the backend service, databases (Redis, SQLite/MySQL), and frontend using Docker for easier deployment and management.

---

## 8. Development & Testing Process

### 8.1 Development Tools
- **Compiler**: GCC / Clang
- **Integrated Development Environment (IDE)**: Visual Studio Code / CLion
- **Version Control**: Git + GitHub / GitLab
- **Redis Client**: `hiredis`
- **Testing Framework**: Google Test / Catch2

### 8.2 Testing Process
- **Unit Testing**: Each module will be independently tested to ensure core functionality.
- **Integration Testing**: Simulate front-to-backend interactions to test API correctness and response times.
- **Performance Testing**: Measure Redis Streams message processing performance and SQLite query performance under high concurrency.

---

## 9. Deployment & Maintenance

### 9.1 Deployment Steps

1. **Prepare Environment**:
   - Ensure the server is equipped with Docker, Redis, and SQLite.
   - If migrating to MySQL, install MySQL.

2. **Compile C++ Backend**:
   - Use `g++` or `clang++` to compile C++ code, and deploy the binary on the server.

3. **Configure Redis**:
   - Deploy Redis and ensure Redis Streams are enabled.
   - Configure Redis memory limits and persistence policies.

4. **Deploy Frontend**:
   - Host static frontend resources (HTML, CSS, JS) on Nginx/Apache.

5. **Start the System**:
   - Use Docker to start Redis and other dependent services.
   - Start the backend services and ensure the APIs are functional.


## 10. Monitoring & Troubleshooting


### 10.1 System Monitoring (continued)

- **Prometheus + Grafana** (continued):
  - Monitor CPU, memory usage, network traffic, Redis Streams message consumption speed, and other key metrics.
  - Grafana will provide real-time visual dashboards, allowing you to monitor system health and performance metrics.

- **Redis Memory Usage Monitoring**:
  - Use Redis’s built-in `INFO` command to continuously monitor memory usage, cache hit rates, and message queue status.
  - Particularly during message peaks, monitor Redis Streams queue length to prevent overflow.

- **Log Management**:
  - Implement centralized logging using the ELK stack (Elasticsearch, Logstash, Kibana).
  - Collect logs from backend services, Redis cache, and database operations to analyze system performance and behavior.

### 10.2 Troubleshooting

- **Redis Failures**:
  - If Redis becomes unavailable, ensure data is restored using persistence mechanisms (AOF or RDB).
  - Monitor Redis message consumption rates to ensure that tasks are resumed after service restoration.

- **SQLite Performance Issues**:
  - For high-concurrency write operations, if performance degrades, consider optimizing SQLite indexes or migrating to MySQL for better performance.

- **Service Restarts**:
  - Use Supervisor or systemd to monitor the C++ backend service and automatically restart it in case of a crash.

---

## 11. Future Iterations & Expansion Plans

### 11.1 Database Expansion

- **Migration to MySQL**:
  - If data volume increases, migrate from SQLite to MySQL for better scalability and concurrency management.
  - MySQL’s advanced transaction management and concurrency control will handle a larger data load more effectively.

- **Distributed Databases**:
  - As the system scales, you can explore distributed databases like CockroachDB or YugabyteDB to handle larger-scale data distribution and sharding.

### 11.2 Message Queue Expansion

- **Kafka Integration**:
  - If Redis Streams can no longer handle the increasing message volume, migrate to Kafka.
  - Kafka will handle larger-scale asynchronous tasks, data streaming, and logging more efficiently with higher throughput.

### 11.3 Service Containerization & Clustering

- **Kubernetes Deployment**:
  - As the system grows, consider containerizing all services and deploying them on a Kubernetes cluster.
  - Kubernetes will automatically handle service scaling, resource management, and high availability.

### 11.4 Frontend Enhancements

- **Frontend Framework Upgrades**:
  - If the system requires more complex user interactions, consider upgrading the frontend to a modern framework like React, Vue.js, or Svelte.
  - This will allow you to build Single Page Applications (SPA) for better user experiences and maintainability.

---

## 12. Conclusion

This development document provides a comprehensive overview of the Pet Store Management System architecture, tech stack, module design, database structure, and message queue usage. Initially, the system will use C++ for backend logic, SQLite and Redis for storage and caching, and Redis Streams for asynchronous task handling. In future iterations, the system can expand to use Kafka for handling large-scale message queues, MySQL for more scalable database management, and Docker/Kubernetes for containerized deployment.

The current architecture and tech stack are designed to be lightweight yet flexible, ensuring smooth operation for a small-scale project, while leaving room for expansion as the system grows in size and complexity.

--- 