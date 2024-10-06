# **ER Diagram 描述**

#### 1. **商品表（Products）**
- **字段**:
    - `product_id` (主键)
    - `name` (商品名称)
    - `price` (商品价格)
    - `stock` (库存数量)
    - `category` (商品类别)
    - `is_deleted` (软删除标记)

- **关联**:
    - 通过 `product_id` 与 `Order_Items` 表关联，表示订单中的商品。
    - 通过 `product_id` 与 `Suppliers` 表关联，表示供应商提供的商品。
    - 通过 `product_id` 与 `Inventory_Actions` 表关联，表示库存变动记录。

---

#### 2. **员工表（Employees）**
- **字段**:
    - `employee_id` (主键)
    - `name` (员工姓名)
    - `position` (职位)
    - `hire_date` (入职日期)
    - `contact_info` (联系信息)
    - `is_deleted` (软删除标记)

- **关联**:
    - 通过 `employee_id` 与 `Orders` 表关联，表示哪个员工处理了订单。

---

#### 3. **订单表（Orders）**
- **字段**:
    - `order_id` (主键)
    - `order_date` (订单日期)
    - `employee_id` (外键，关联 `Employees.employee_id`)
    - `customer_id` (外键，关联 `Customers.customer_id`)  -- 直接关联客户表，表示哪个客户下了订单
    - `total` (订单总价)
    - `status` (订单状态)
    - `is_deleted` (软删除标记)

- **关联**:
    - `Orders` 表通过 `order_id` 与 `Order_Items` 表关联，表示订单的商品明细。
    - 通过 `customer_id` 关联 `Customers` 表，表示哪个客户下了该订单。

---

#### 4. **订单商品关联表（Order_Items）**
- **字段**:
    - `order_item_id` (主键)
    - `order_id` (外键，关联 `Orders.order_id`)
    - `product_id` (外键，关联 `Products.product_id`)
    - `quantity` (商品数量)
    - `price` (商品单价)
    - `is_deleted` (软删除标记)

- **关联**:
    - `Order_Items` 表通过 `order_id` 关联 `Orders` 表，表示订单中的商品。
    - 通过 `product_id` 关联 `Products` 表，表示具体商品的明细。

---

#### 5. **客户表（Customers）**
- **字段**:
    - `customer_id` (主键)
    - `name` (客户姓名)
    - `phone_number` (电话号码)
    - `email` (电子邮件)
    - `address` (地址)
    - `is_deleted` (软删除标记)

- **关联**:
    - `Customers` 表通过 `customer_id` 直接与 `Orders` 表关联，表示哪个客户下了订单。

---

#### 6. **供应商表（Suppliers）**
- **字段**:
    - `supplier_id` (主键)
    - `name` (供应商名称)
    - `contact_info` (联系信息)
    - `product_id` (外键，关联 `Products.product_id`)
    - `is_deleted` (软删除标记)

- **关联**:
    - 通过 `product_id` 关联 `Products` 表，表示供应商提供哪些商品。

---

#### 7. **库存管理表（Inventory_Actions）**
- **字段**:
    - `action_id` (主键)
    - `product_id` (外键，关联 `Products.product_id`)
    - `action_type` (库存变动类型，如入库、出库)
    - `quantity` (数量变动)
    - `action_date` (操作日期)
    - `is_deleted` (软删除标记)

- **关联**:
    - 通过 `product_id` 关联 `Products` 表，记录商品的库存变动。

---

### 软删除机制：
- 每个表中都有 `is_deleted` 字段，默认为 `FALSE`。当需要“删除”记录时，设置 `is_deleted` 为 `TRUE`，而不是实际删除数据。
- 查询未删除的数据时，可以通过过滤 `is_deleted = FALSE` 来只查询有效数据。

---

### 实体关系描述（更新后）：
- **Products (商品表)**:
    - 通过 `Order_Items` 表与 `Orders` 表关联，表示订单中包含的商品。
    - 通过 `Suppliers` 表与供应商关联，表示供应商提供的商品。
    - 通过 `Inventory_Actions` 表与库存管理关联，记录商品的库存变动。

- **Employees (员工表)**:
    - 与 `Orders` 表关联，表示员工处理的订单。

- **Customers (客户表)**:
    - 通过 `customer_id` 与 `Orders` 表关联，表示哪个客户下了订单。

- **Suppliers (供应商表)**:
    - 与 `Products` 表关联，表示供应商提供的商品。

- **Inventory_Actions (库存管理表)**:
    - 与 `Products` 表关联，记录商品的库存变化。

---
