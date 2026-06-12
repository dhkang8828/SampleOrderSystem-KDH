# Class Design: ProductionQueueRepository

## 개요

ProductionJob 도메인의 SQLite 영속성 처리 담당.  
FIFO 스케줄링 기반의 생산 큐를 관리한다.

## 확장성 설계 (IScheduler)

```cpp
class IScheduler {
public:
    virtual ~IScheduler() = default;
    virtual std::optional<ProductionJob> next() = 0;  // 다음 작업 반환
    virtual void enqueue(const ProductionJob& job) = 0;
    virtual std::vector<ProductionJob> getQueue() const = 0;
};

// 현재 구현체
class FIFOScheduler : public IScheduler { ... };

// 향후 확장 가능
// class PriorityScheduler : public IScheduler { ... };
```

- `ProductionQueueRepository`는 `IScheduler`를 구현하여 DB 기반 FIFO 큐 제공

## 클래스 정의

```cpp
class ProductionQueueRepository : public IRepository<ProductionJob>,
                                   public IScheduler {
public:
    explicit ProductionQueueRepository(DBManager& dbManager);

    // IRepository<ProductionJob> 구현
    bool save(const ProductionJob& job) override;
    std::optional<ProductionJob> findById(const std::string& id) override;
    std::vector<ProductionJob> findAll() override;
    bool update(const ProductionJob& job) override;
    bool remove(const std::string& id) override;

    // IScheduler 구현 (FIFO)
    std::optional<ProductionJob> next() override;       // 가장 오래된 작업 조회
    void enqueue(const ProductionJob& job) override;    // 큐 등록
    std::vector<ProductionJob> getQueue() const override; // 대기 목록 전체

    // 추가 메서드
    std::optional<ProductionJob> findByOrderId(const std::string& orderId);
    bool removeByOrderId(const std::string& orderId);   // 생산 완료 후 제거

private:
    DBManager& dbManager_;
    ProductionJob rowToJob(sqlite3_stmt* stmt) const;
};
```

## SQLite 테이블 스키마

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

## FIFO 동작

```
enqueue → queue_id AUTO INCREMENT으로 순서 보장
next    → SELECT WHERE queue_id = MIN(queue_id) → 가장 먼저 들어온 작업 반환
```
