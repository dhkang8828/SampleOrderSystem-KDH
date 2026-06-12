# Class Design: OrderRepository

## 개요

Order 도메인의 SQLite 영속성 처리 담당.  
`IRepository<Order>`를 상속하여 CRUD 및 상태별 조회를 구현한다.

## 클래스 정의

```cpp
class OrderRepository : public IRepository<Order> {
public:
    explicit OrderRepository(DBManager& dbManager);

    // IRepository<Order> 구현
    bool save(const Order& order) override;
    std::optional<Order> findById(const std::string& orderId) override;
    std::vector<Order> findAll() override;
    bool update(const Order& order) override;
    bool remove(const std::string& orderId) override;

    // 추가 조회 메서드
    std::vector<Order> findByStatus(OrderStatus status);          // 상태별 조회
    std::vector<Order> findBySampleId(const std::string& sampleId); // 시료별 조회
    bool updateStatus(const std::string& orderId, OrderStatus newStatus); // 상태만 변경
    int countByStatus(OrderStatus status);                        // 상태별 주문 수
    int generateNextOrderSequence();                              // 주문번호 시퀀스 생성

private:
    DBManager& dbManager_;
    Order rowToOrder(sqlite3_stmt* stmt) const;
    std::string orderStatusToString(OrderStatus status) const;
    OrderStatus stringToOrderStatus(const std::string& str) const;
};
```

## SQLite 테이블 스키마

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

## 주요 메서드 동작

| 메서드 | SQL | 설명 |
|---|---|---|
| `save` | INSERT INTO orders | 주문 신규 저장 |
| `findById` | SELECT WHERE order_id = ? | 주문번호로 단건 조회 |
| `findAll` | SELECT * FROM orders | 전체 주문 목록 |
| `update` | UPDATE orders SET ... | 전체 필드 업데이트 |
| `findByStatus` | SELECT WHERE status = ? | RESERVED/CONFIRMED 등 상태별 |
| `updateStatus` | UPDATE SET status = ? | 상태만 변경 |
| `countByStatus` | SELECT COUNT(*) WHERE status = ? | 모니터링용 집계 |
| `generateNextOrderSequence` | SELECT COUNT(*) + 1 | ORD-YYYYMMDD-NNNN 시퀀스 |

## 주문번호 생성 규칙

```
형식: ORD-YYYYMMDD-NNNN
예시: ORD-20260612-0001

NNNN = 해당 날짜의 주문 순번 (4자리, 0001부터 시작)
```
