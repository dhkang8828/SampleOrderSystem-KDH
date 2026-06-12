# Class Design: Repository Layer

## 포함 클래스
- IRepository\<T\> (추상 인터페이스)
- SampleRepository
- OrderRepository
- ProductionQueueRepository
- IScheduler (추상 인터페이스)

---

## 1. IRepository\<T\>

### 개요
모든 Repository가 상속하는 추상 인터페이스.
Controller는 인터페이스에만 의존하여 DB 교체 및 Mock 주입이 용이하다. (DIP 원칙)

### 확장성 설계
```
IRepository<T>
  ├── SampleRepository          (SQLite 구현체)
  ├── OrderRepository           (SQLite 구현체)
  └── ProductionQueueRepository (SQLite 구현체)

향후 교체 시:
  └── SampleMockRepository      (테스트용 Mock)
```

### 클래스 정의
```cpp
template <typename T>
class IRepository {
public:
    virtual ~IRepository() = default;

    virtual bool save(const T& entity) = 0;
    virtual std::optional<T> findById(const std::string& id) = 0;
    virtual std::vector<T> findAll() = 0;
    virtual bool update(const T& entity) = 0;
    virtual bool remove(const std::string& id) = 0;
};
```

---

## 2. SampleRepository

### 클래스 정의
```cpp
class SampleRepository : public IRepository<Sample> {
public:
    explicit SampleRepository(DBManager& dbManager);

    bool save(const Sample& sample) override;
    std::optional<Sample> findById(const std::string& sampleId) override;
    std::vector<Sample> findAll() override;
    bool update(const Sample& sample) override;
    bool remove(const std::string& sampleId) override;

    std::vector<Sample> findByName(const std::string& keyword);
    bool updateStock(const std::string& sampleId, int newStock);
    bool existsById(const std::string& sampleId);

private:
    DBManager& dbManager_;
    Sample rowToSample(sqlite3_stmt* stmt) const;
};
```

### SQLite 테이블 스키마
```sql
CREATE TABLE IF NOT EXISTS samples (
    sample_id            TEXT PRIMARY KEY,
    name                 TEXT NOT NULL,
    avg_production_time  REAL NOT NULL,
    yield_rate           REAL NOT NULL,
    stock                INTEGER NOT NULL DEFAULT 0
);
```

### 주요 메서드 동작
| 메서드 | SQL | 설명 |
|---|---|---|
| `save` | INSERT INTO samples | 중복 ID 시 false 반환 |
| `findById` | SELECT WHERE sample_id = ? | 없으면 nullopt |
| `findAll` | SELECT * FROM samples | 전체 시료 목록 |
| `update` | UPDATE samples SET ... | 전체 필드 업데이트 |
| `findByName` | SELECT WHERE name LIKE ? | 부분 일치 검색 |
| `updateStock` | UPDATE SET stock = ? | 재고만 변경 |
| `existsById` | SELECT COUNT(*) WHERE ... | 존재 여부 확인 |

---

## 3. OrderRepository

### 클래스 정의
```cpp
class OrderRepository : public IRepository<Order> {
public:
    explicit OrderRepository(DBManager& dbManager);

    bool save(const Order& order) override;
    std::optional<Order> findById(const std::string& orderId) override;
    std::vector<Order> findAll() override;
    bool update(const Order& order) override;
    bool remove(const std::string& orderId) override;

    std::vector<Order> findByStatus(OrderStatus status);
    std::vector<Order> findBySampleId(const std::string& sampleId);
    bool updateStatus(const std::string& orderId, OrderStatus newStatus);
    int countByStatus(OrderStatus status);
    int generateNextOrderSequence();

private:
    DBManager& dbManager_;
    Order rowToOrder(sqlite3_stmt* stmt) const;
    std::string orderStatusToString(OrderStatus status) const;
    OrderStatus stringToOrderStatus(const std::string& str) const;
};
```

### SQLite 테이블 스키마
```sql
CREATE TABLE IF NOT EXISTS orders (
    order_id       TEXT PRIMARY KEY,
    sample_id      TEXT NOT NULL,
    customer_name  TEXT NOT NULL,
    quantity       INTEGER NOT NULL,
    status         TEXT NOT NULL,
    created_at     TEXT NOT NULL,
    FOREIGN KEY (sample_id) REFERENCES samples(sample_id)
);
```

### 주문번호 생성 규칙
```
형식: ORD-YYYYMMDD-NNNN
예시: ORD-20260612-0001
NNNN = 해당 날짜의 주문 순번 (4자리, 0001부터 시작)
```

---

## 4. ProductionQueueRepository

### IScheduler 인터페이스
```cpp
class IScheduler {
public:
    virtual ~IScheduler() = default;
    virtual std::optional<ProductionJob> next() = 0;
    virtual void enqueue(const ProductionJob& job) = 0;
    virtual std::vector<ProductionJob> getQueue() const = 0;
};

// 현재 구현: FIFOScheduler
// 향후 확장: PriorityScheduler, RoundRobinScheduler 등
```

### 클래스 정의
```cpp
class ProductionQueueRepository : public IRepository<ProductionJob>,
                                   public IScheduler {
public:
    explicit ProductionQueueRepository(DBManager& dbManager);

    bool save(const ProductionJob& job) override;
    std::optional<ProductionJob> findById(const std::string& id) override;
    std::vector<ProductionJob> findAll() override;
    bool update(const ProductionJob& job) override;
    bool remove(const std::string& id) override;

    std::optional<ProductionJob> next() override;         // 가장 오래된 작업
    void enqueue(const ProductionJob& job) override;      // 큐 등록
    std::vector<ProductionJob> getQueue() const override; // 대기 목록 전체

    std::optional<ProductionJob> findByOrderId(const std::string& orderId);
    bool removeByOrderId(const std::string& orderId);

private:
    DBManager& dbManager_;
    ProductionJob rowToJob(sqlite3_stmt* stmt) const;
};
```

### SQLite 테이블 스키마
```sql
CREATE TABLE IF NOT EXISTS production_queue (
    queue_id               INTEGER PRIMARY KEY AUTOINCREMENT,
    order_id               TEXT NOT NULL,
    actual_production_qty  INTEGER NOT NULL,
    total_production_time  REAL NOT NULL,
    enqueued_at            TEXT NOT NULL,
    FOREIGN KEY (order_id) REFERENCES orders(order_id)
);
```

### FIFO 동작
```
enqueue → queue_id AUTOINCREMENT으로 순서 보장
next    → SELECT WHERE queue_id = MIN(queue_id) → 가장 먼저 들어온 작업
```
